
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "Shlwapi.lib")

#include "main.h"

#include <shellapi.h>
#include <Commdlg.h>
#include <codecvt>
#include <Dbghelp.h>
#include <string>
#include <algorithm>
#include <clocale>

#include "minldr.h"
#include "util.h"

void Initialize(RSDSEntry& gameInfo);

static void GetAddressName(char* buffer, size_t bufferSize, intptr_t address)
{
	bool unknown = true;

#if 0
	module_info_t meminfo;
	mymodule.GetModuleInfo(meminfo);

	if (meminfo.instance != nullptr) {
		if ((address >= meminfo.begin) && (address <= meminfo.end)) {
			intptr_t relAddr = (address - meminfo.begin);

			sprintf(buffer, "%08X (D3Hook+%08X)", address, relAddr);

			unknown = false;
		}
	}
#endif

	if (unknown)
		sprintf_s(buffer, bufferSize, "%08X (Unknown)", address);
}

void WINAPI GetStartupInfoA_Stub(LPSTARTUPINFOA lpStartupInfo)
{
	GetStartupInfoA(lpStartupInfo);

	static bool initGuard = false;
	if (!initGuard) {
		RSDSEntry gameInfo{};
		if (CDriv3r::GetGameInfo(gameInfo)) {
			LogFile::Format(" - Detected game version %1.2f\n", gameInfo.fancy_version);

			Initialize(gameInfo);
			initGuard = true;
		}
	}
}

// as the game fetches its own path via command line,
// we supply a fake command line (see +0x005A6937)
const char* WINAPI GetCommandLineA_Fake()
{
	static std::string fakeCmdl;

	if (fakeCmdl.empty()) {
		using convert_type = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;
		fakeCmdl = converter.to_bytes(makeGamePath(L"notfake.exe"));
	}

	return fakeCmdl.c_str();
}

LONG WINAPI CustomUnhandledExceptionFilter(LPEXCEPTION_POINTERS ExceptionInfo)
{
    if (!ExceptionInfo || !ExceptionInfo->ExceptionRecord)
        return EXCEPTION_CONTINUE_SEARCH;

    auto record = ExceptionInfo->ExceptionRecord;

    auto address = record->ExceptionAddress;
    auto code = record->ExceptionCode;
    auto flags = record->ExceptionFlags;

    char buffer[512] = { NULL };
    GetAddressName(buffer, sizeof(buffer), reinterpret_cast<intptr_t>(address));

    LogFile::Format("[CRASH] %08X at %s : %X : %p (%d)\n", code, buffer, flags,
        record->ExceptionInformation, record->NumberParameters);

    char message[512] = { NULL };
    sprintf_s(message, "The game has crashed at %s. Press OK to continue execution.", buffer);

    MessageBoxA(NULL, message, "D3Hook", MB_ICONERROR | MB_OK);

    if (record->NumberParameters > 0)
        RaiseException(code, flags, record->NumberParameters, record->ExceptionInformation);

    return EXCEPTION_EXECUTE_HANDLER;
}

static LONG NTAPI HandleVariant(PEXCEPTION_POINTERS exceptionInfo)
{
	SetForegroundWindow(GetDesktopWindow());
	return (exceptionInfo->ExceptionRecord->ExceptionCode == STATUS_INVALID_HANDLE)
		? EXCEPTION_CONTINUE_EXECUTION
		: EXCEPTION_CONTINUE_SEARCH;
}

static void invokeEntry(void(*ep)())
{
	// SEH call to prevent STATUS_INVALID_HANDLE
	__try
	{
		// and call the entry point
		ep();
	}
	__except (HandleVariant(GetExceptionInformation()))
	{
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	LogFile::WriteLine("Initializing D3Hook...");

	auto gamePath = makeGamePath(L"");

	auto* hkernel = GetModuleHandleW(L"kernel32.dll");
	auto _AddDllDirectory = (decltype(&AddDllDirectory))GetProcAddress(hkernel, "AddDllDirectory");
	auto _SetDefaultDllDirectories = (decltype(&SetDefaultDllDirectories))GetProcAddress(hkernel, "SetDefaultDllDirectories");

	/*w 8+ only*/
	if (_AddDllDirectory && _SetDefaultDllDirectories) {

		_SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_USER_DIRS);
		_AddDllDirectory(gamePath.c_str());	
		_AddDllDirectory(makeToolPath(L"").c_str());
	}

	SetCurrentDirectoryW(gamePath.c_str());
	SetDllDirectoryW(gamePath.c_str());

	LogFile::Format("Game directory is '%S'\n", gamePath.c_str());

	FILE* game_file = nullptr;
	_wfopen_s(&game_file, makeGamePath(L"Driv3r.exe").c_str(), L"rb");

	if (!game_file)
		return -1;

	fseek(game_file, 0, SEEK_END);

	uint32_t length = ftell(game_file);
	if (length != 0x003f2000) {
		MessageBoxW(nullptr, L"Sorry, this version of Driv3r is unsupported. Please remove D3Hook to launch the game.", L"D3Hook", MB_OK | MB_ICONERROR);
		fclose(game_file);
		return -1;
	}

	uint8_t* data = new uint8_t[length];

	fseek(game_file, 0, SEEK_SET);
	fread(data, 1, length, game_file);

	fclose(game_file);

	const HMODULE exe_module = GetModuleHandle(nullptr);

	minLdr exeLoader(data);

	exeLoader.setLibLoader([](const char* name) {
		return LoadLibraryA(name);
	});

	exeLoader.setFuncResolver([](HMODULE mod, const char* name) {

		/*our code entry point*/
		if (!_stricmp(name, "GetStartupInfoA"))
			return (LPVOID)&GetStartupInfoA_Stub;

		if (!_stricmp(name, "GetCommandLineA"))
			return (LPVOID)&GetCommandLineA_Fake;

		return (LPVOID)GetProcAddress(mod, name);
	});

	if (!exeLoader.load(exe_module))
		return -1;

	// free the old binary
	delete[] data;

	DWORD oldProtect;
	VirtualProtect(reinterpret_cast<void*>(0x401000), length,
		PAGE_EXECUTE_READWRITE, &oldProtect);

	auto entry = static_cast<void(*)()>(exeLoader.GetEntryPoint());

	AddVectoredExceptionHandler(0, HandleVariant);
	SetUnhandledExceptionFilter(CustomUnhandledExceptionFilter);
	invokeEntry(entry);
	return 0;
}