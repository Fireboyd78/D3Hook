#include "common.h"
#include "fiero.h"

#ifdef FX_DEBUG
static char g_log_buffer[4096 + 1] = { NULL };

constexpr size_t sizeof_log = sizeof(g_log_buffer) - 1;

void fxDebugf(LPCSTR format, ...) {
    va_list va;
    va_start(va, format);
    vsnprintf(g_log_buffer, sizeof_log, format, va);
    va_end(va);

    size_t length = strnlen(g_log_buffer, sizeof_log);

    // oh no, the world is ending!
    g_log_buffer[length] = '\n';
    g_log_buffer[length + 1] = '\0';

    OutputDebugStringA(g_log_buffer);
    LogFile::Format(g_log_buffer);
}
#endif

/*
    fxModule
*/

fxModule::fxModule(LPCSTR libName)
    : fxModule(libName, fxModuleLoadType::Normal) {}

fxModule::fxModule(LPCSTR libName, fxModuleLoadType loadType)
    : fxModule(LoadLibraryExA(libName, NULL, static_cast<DWORD>(loadType))) {}

fxModule::fxModule(LPCWSTR libName)
    : fxModule(libName, fxModuleLoadType::Normal) {}

fxModule::fxModule(LPCWSTR libName, fxModuleLoadType loadType)
    : fxModule(LoadLibraryExW(libName, NULL, static_cast<DWORD>(loadType))) {}

/* static */
bool fxModule::Initialize(fxModule &module, HINSTANCE instance) {
    return module.InitModuleInfo(instance);
}

bool fxModule::InitModuleInfo(HINSTANCE instance) {
    if (instance == NULL)
        instance = GetModuleHandle(NULL);

    fx_assert(instance != nullptr, "Module not loaded!");

    intptr_t base = reinterpret_cast<intptr_t>(instance);

    PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(instance);

    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        fx_error_("Invalid DOS signature", std::runtime_error);
        return false;
    }

    PIMAGE_NT_HEADERS peHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(base + dosHeader->e_lfanew);

    if (peHeader->Signature != IMAGE_NT_SIGNATURE) {
        fx_error_("Invalid NT Signature", std::runtime_error);
        return false;
    }

    this->instance = instance;

    this->begin = base;
    this->end = base + (peHeader->OptionalHeader.BaseOfCode + peHeader->OptionalHeader.SizeOfCode);
    this->size = peHeader->OptionalHeader.SizeOfImage;

    this->sections = {};
    this->imports = {};
    this->exports = {};

    /*
        Process sections
    */

    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(peHeader);

    for (int i = 0; i < peHeader->FileHeader.NumberOfSections; i++, section++)
    {
        bool executable = (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
        bool readable = (section->Characteristics & IMAGE_SCN_MEM_READ) != 0;
        bool writeable = (section->Characteristics & IMAGE_SCN_MEM_WRITE) != 0;

        if (readable && executable) {
            auto s_begin = peHeader->OptionalHeader.ImageBase + section->VirtualAddress;
            auto s_size = section->Misc.VirtualSize;

            if (s_size > section->SizeOfRawData)
                s_size = section->SizeOfRawData;

            auto s_info = section_info_t(s_begin, (s_begin + s_size));

            sections.emplace_back(s_info);
        }
    }

    /*
        Process imports
    */

    PIMAGE_DATA_DIRECTORY importDir = &peHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

    if (importDir->Size != 0)
    {
        PIMAGE_IMPORT_DESCRIPTOR importDesc = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(base + importDir->VirtualAddress);

        for (; !IsBadReadPtr(importDesc, sizeof(IMAGE_IMPORT_DESCRIPTOR)) && importDesc->Name; importDesc++)
        {
            auto libName = reinterpret_cast<LPCSTR>(base + importDesc->Name);
            
            import_descriptor_t desc = {
                reinterpret_cast<intptr_t>(importDesc),
                libName
            };

            DWORD funcref_ptr = importDesc->FirstThunk;
            DWORD thunkref_ptr = importDesc->OriginalFirstThunk;

            // no hint table?
            if (thunkref_ptr == NULL)
                thunkref_ptr = importDesc->FirstThunk;

            FARPROC *func_ref = reinterpret_cast<FARPROC *>(base + funcref_ptr);
            intptr_t *thunk_ref = reinterpret_cast<intptr_t *>(base + thunkref_ptr);

            for (; *thunk_ref, *func_ref; thunk_ref++, func_ref++) {
                // can't work with ordinals yet
                if (!IMAGE_SNAP_BY_ORDINAL(*thunk_ref)) {
                    auto importInfo = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(base + *thunk_ref);

                    import_info_t info = {
                        reinterpret_cast<intptr_t>(importInfo),
                        reinterpret_cast<LPCSTR>(&importInfo->Name),
                        func_ref,
                        thunk_ref,
                    };

                    desc.infos.emplace_back(info);
                }
            }

            imports.emplace_back(desc);
        }
    }

    /*
        Process exports
    */

    PIMAGE_DATA_DIRECTORY exportDir = &peHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

    if (exportDir->Size != 0)
    {
        PIMAGE_EXPORT_DIRECTORY exportDesc = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(base + exportDir->VirtualAddress);

        PDWORD functions = reinterpret_cast<PDWORD>(base + exportDesc->AddressOfFunctions);
        PDWORD names = reinterpret_cast<PDWORD>(base + exportDesc->AddressOfNames);

        for (uint32_t f = 0, n = 0; (f < exportDesc->NumberOfFunctions) && (n < exportDesc->NumberOfNames); f++, n++) {
            auto func_ptr = &functions[f];
            auto name_ptr = &names[n];

            // skip exports we don't care about
            if ((*func_ptr == NULL) || (*name_ptr == NULL))
                continue;

            auto name = reinterpret_cast<LPCSTR>(base + *name_ptr);

            export_descriptor_t desc = {
                func_ptr,
                name,
            };

            exports.emplace_back(desc);
        }
    }

    return true;
}

fxModule::fxModule(HINSTANCE instance) {
    this->InitModuleInfo(instance);
}

fxModule::~fxModule() { /* do nothing */ }

void fxModule::GetModuleInfo(module_info_t &moduleInfo) {
    // fxModule inherits from a module_info_t, so we can copy it
    moduleInfo = *this;
}

BOOL fxModule::Free() {
    if (instance == nullptr)
        return TRUE;

    BOOL result = FreeLibrary(instance);

    instance = nullptr;

    return result;
}

auto_ptr fxModule::FindImport(LPCSTR fnName) {
    if (instance == nullptr)
        return nullptr;

    for (auto &desc : imports) {
        for (auto &info : desc.infos) {
            if (_stricmp(info.name, fnName) == 0) {
                return reinterpret_cast<intptr_t>(instance) + reinterpret_cast<intptr_t>(*info.func_ref);
            }
        }
    }

    return nullptr;
}

auto_ptr fxModule::FindExport(LPCSTR fnName) {
    if (instance == nullptr)
        return nullptr;

    for (auto &desc : exports) {
        if (_stricmp(desc.name, fnName) == 0) {
            return reinterpret_cast<intptr_t>(instance) + *desc.func_ptr;
        }
    }

    return nullptr;
}

/*
auto_ptr fxModule::ReplaceImport(LPCSTR fnName, auto_ptr target) {
    if (instance == nullptr)
        return nullptr;

    intptr_t base = reinterpret_cast<intptr_t>(instance);

    for (auto &desc : imports) {
        for (auto &info : desc.infos) {
            if (_stricmp(info.name, fnName) == 0) {
                auto_ptr func_ptr(base + reinterpret_cast<intptr_t>(*info.func_ref));
                fxDebugf("replacing import '%s' (%016llX -> %016llX)", fnName, func_ptr, target);
                return nhx::Detour(func_ptr, target);
            }
        }
    }

    return nullptr;
}

auto_ptr fxModule::ReplaceExport(LPCSTR fnName, auto_ptr target) {
    if (instance == nullptr)
        return nullptr;

    intptr_t base = reinterpret_cast<intptr_t>(instance);

    for (auto &desc : exports) {
        if (_stricmp(desc.name, fnName) == 0) {
            auto_ptr func_ptr(base + static_cast<intptr_t>(*desc.func_ptr));
            fxDebugf("replacing export '%s' (%016llX -> %016llX)", fnName, func_ptr, target);
            return nhx::Detour(func_ptr, target);
        }
    }

    return nullptr;
}
*/