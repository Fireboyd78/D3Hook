#include "hooksystem.h"
#include "driv3r.h"

uint8_t hook_ops[hookType::COUNT] = {
    0xE9,
    0xE8,
    0x68,
};

LPCSTR hook_types[hookType::COUNT] = {
    "jmp",
    "call",
    "push"
};

auto test = pack({ 1, 2, 3 });

static init_base *g_initializers;

void init_base::Register() {
    this->next = g_initializers;
    g_initializers = this;
};

void init_base::RunAll() {
    for (auto func = g_initializers; func != nullptr; func = func->next) {
        func->Run();
    }
};

void InstallHandler(LPCSTR name, init_t installHandler) {
    LogFile::Format("Installing '%s' handler...\n", name);

    if (!installHandler())
        LogFile::Format("**** Not supported\n");
};

void InstallPatch(LPCSTR description, std::initializer_list<uint8_t> bytes, std::initializer_list<intptr_t> addresses)
{
    const auto begin = bytes.begin();
    const auto size = bytes.size();

    LogFile::Format(" - Installing patch [%08X : %08X]:\n", begin, size);

    if (description != nullptr)
        LogFile::Format(" - Description: %s\n", description);

    for (auto addr : addresses)
    {
        LogFile::Format("   => %08X\n", addr);
        mem::copy(addr, begin, size);
    }
}

int inject_callback(uint8_t *dest, intptr_t rva, uint8_t op, intptr_t reloc, intptr_t *cb) {
    int type = 0; // unknown, default, push, absolute indirect

    uint8_t kind = *dest;
    intptr_t orig = 0;

    LogFile::Format("**** inject %X (%X %X)\n", dest, op, reloc);
    LogFile::Format("  kind %X", kind);

    switch (kind)
    {
    case 0xFF: /* call/jmp to pointer */
        orig = *(intptr_t *)(dest + 2); // step 1: grab the pointer
        type = 3;

        LogFile::Format(" (absolute indirect)\n");
        break;
    case 0x68: /* push */
        orig = *(intptr_t *)(dest + 1);
        type = 2;

        LogFile::Format(" (push)\n");
        break;
    case 0xE8: /* jmp */
    case 0xE9: /* call */
        orig = *(intptr_t *)(dest + 1) + rva;
        type = 1;

        LogFile::Format(" (default)\n");
        break;
    default: /* who knows? */
        LogFile::Format(" (unknown)\n");
        break;
    }
    
    LogFile::Format("  reloc %X", orig);
    if (type == 3)
    {
        orig = *(intptr_t *)orig; // step 2: resolve the pointer
        LogFile::Format(" (-> %X)", orig);
    }
    LogFile::Format("\n");

    LogFile::Format("  inject %X %X", op, reloc);
    *dest = op;
    *(intptr_t *)(dest + 1) = reloc;
    
    // nop required?
    if (type == 3)
    {
        LogFile::Format(" nop");
        *(dest + 5) = 0x90;
    }

    LogFile::Format("\n");

    if (type && (cb != nullptr))
        *cb = orig;

    return type;
}

void InstallCallback(auto_ptr target, cbInfo info, intptr_t *cb)
{
    auto addr = info.addr;
    auto type = info.type;

    switch (type)
    {
    case hookType::CALL:
    case hookType::JMP:
    case hookType::PUSH:
        {
            intptr_t rva = addr;
            intptr_t reloc = target;

            if (type != hookType::PUSH)
            {
                rva += 5;
                reloc -= rva;
            }
            /*
            **
            */
            {
                mem::scoped_protector xmem(addr, 6, PAGE_EXECUTE_READWRITE);

                uint8_t op = hook_ops[type];

            #ifndef USE_OLD_BROKEN_CODE
                inject_callback(addr, rva, op, reloc, cb);
            #else
                uint8_t kind = *dest.ptr<uint8_t>();

                // the original callback
                intptr_t orig_reloc = 0;
                int orig_type = 0; // default, push, direct
                

                LogFile::Format("**** install %X %X (%X %X)\n", dest, addr, rva, reloc);
                LogFile::Format("  kind %X\n", kind);

                if (kind == 0xFF /* call/jmp to pointer */)
                {
                    // steal that shit directly
                    orig_reloc = **dest.ptr<intptr_t *>(2);
                    orig_type = 2;

                    LogFile::Format("  write %X %X nop\n", op, reloc);
                    mem::write<uint8_t, intptr_t, uint8_t>(xmem, op, reloc, 0x90);
                }
                else
                {
                    if (kind == 0x68 /* push */)
                    {
                        // no need to add rva
                        orig_reloc = *dest.ptr<intptr_t>(1);
                        orig_type = 1;
                    }
                    else
                    {
                        // NO ERROR CHECKING BEYOND THIS POINT
                        orig_reloc = *dest.ptr<intptr_t>(1) + rva;
                        orig_type = 2;
                    }

                    LogFile::Format("  write %X %X\n", op, reloc);
                    mem::write<uint8_t, intptr_t>(xmem, op, reloc);
                }

                LogFile::Format("  old reloc %X %d\n", orig_reloc, orig_type);
                LogFile::Format("**** result %X %X\n", *(uint8_t*)dest, *(intptr_t*)(dest + 1));
            #endif
            }
        } break;
    }
}

void InstallCallback(LPCSTR description, auto_ptr hook, cbInfo info, intptr_t *cb)
{
    LogFile::Format(" - Installing callback [%08X]:\n", hook);

    if (description != nullptr)
        LogFile::Format(" - Description: %s\n", description);

    LogFile::Format("   => [%s] %08X\n", hook_types[info.type], info.addr);
    InstallCallback(hook, info, cb);
}

void InstallCallback(LPCSTR name, LPCSTR description, auto_ptr hook, cbInfo info, intptr_t *cb)
{
    LogFile::Format(" - Installing callback [%08X] for '%s':\n", hook, name);

    if (description != nullptr)
        LogFile::Format(" - Description: %s\n", description);

    LogFile::Format("   => [%s] %08X\n", hook_types[info.type], info.addr);
    InstallCallback(hook, info, cb);
}

void InstallCallback(LPCSTR name, LPCSTR description, auto_ptr hook, std::initializer_list<cbInfo> callbacks)
{
    LogFile::Format(" - Installing callback [%08X] for '%s':\n", hook, name);

    if (description != nullptr)
        LogFile::Format(" - Description: %s\n", description);

    for (auto &info : callbacks)
    {
        LogFile::Format("   => [%s] %08X\n", hook_types[info.type], info.addr);
        InstallCallback(hook, info, nullptr);
    }
}

void InstallVTableHook(LPCSTR name, auto_ptr hook, std::initializer_list<intptr_t> addresses)
{
    LogFile::Format(" - Installing V-Table hook [%08X]: '%s'...\n", hook, name);

    for (auto addr : addresses)
    {
        LogFile::Format("   => %08X\n", addr, hook);
        mem::write<intptr_t>(addr, hook);
    }
}