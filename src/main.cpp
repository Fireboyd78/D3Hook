
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

static ConfigValue<bool> cfgShowConsole         ("ShowConsole", true);
static ConfigValue<bool> cfgDebugLog            ("DebugLog", 	true);

static ConfigValue<bool> cfgDebugMode           ("DebugMode", 	true);

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

LONG WINAPI CustomUnhandledExceptionFilter(LPEXCEPTION_POINTERS ExceptionInfo)
{
	if (!ExceptionInfo || !ExceptionInfo->ExceptionRecord)
		return EXCEPTION_CONTINUE_SEARCH;

	auto record = ExceptionInfo->ExceptionRecord;

	auto address = record->ExceptionAddress;
	auto code = record->ExceptionCode;
	auto flags = record->ExceptionFlags;

	if (code == STATUS_INVALID_HANDLE)
		return EXCEPTION_CONTINUE_EXECUTION;

	char buffer[512] = { NULL };
	GetAddressName(buffer, sizeof(buffer), reinterpret_cast<intptr_t>(address));

	LogFile::Format("[CRASH] %08X at %s : %X : %p (%d)\n", code, buffer, flags,
		record->ExceptionInformation, record->NumberParameters);

	char message[512] = { NULL };
	sprintf_s(message, "The game has crashed at %s. Press OK to continue execution.", buffer);

	MessageBoxA(NULL, message, "D3Hook", MB_ICONERROR | MB_OK);

	//if (record->NumberParameters > 0)
	//    RaiseException(code, flags, record->NumberParameters, record->ExceptionInformation);

	return EXCEPTION_CONTINUE_SEARCH;
}

static LONG NTAPI HandleVariant(PEXCEPTION_POINTERS exceptionInfo)
{
	SetForegroundWindow(GetDesktopWindow());
	return (exceptionInfo->ExceptionRecord->ExceptionCode == STATUS_INVALID_HANDLE)
		? EXCEPTION_CONTINUE_EXECUTION
		: EXCEPTION_CONTINUE_SEARCH;
}

#if defined(HOOK_DLL)
extern char gamedir[MAX_PATH];

extern int gameversion;

extern fxModule gamemodule;
extern fxModule mymodule;

bool InitConfigFile()
{
	bool configLoaded;

#ifdef HOOK_EXE
	configLoaded = HookConfig::Initialize(makeToolPath(L"d3hook.ini").c_str());
#else
	configLoaded = HookConfig::Initialize("d3hook.ini");
#endif

	if (cfgShowConsole) {
		ConsoleLog::Initialize();
		ConsoleLog::SetTitle("D3Hook Console");
	}

	return configLoaded;
}

void InitPath(void) {
	char dir[MAX_PATH]{ NULL };
	auto len = GetModuleFileName(NULL, dir, MAX_PATH);

	if (GetPathSpec(dir, gamedir, len)) {
		SetCurrentDirectory(gamedir);
	} else {
		GetCurrentDirectory(MAX_PATH, gamedir);
	}
}

bool IsGameSupported(RSDSEntry &gameInfo) {
	LogFile::WriteLine("Checking for known Driv3r versions...");

	if (CDriv3r::GetGameInfo(gameInfo))
	{
		LogFile::Format(" - Detected game version %1.2f\n", gameInfo.fancy_version);
		return gameInfo.isSupported;
	} else {
		LogFile::WriteLine("Unknown module detected! Terminating...");
		MessageBox(NULL, "Unknown module! D3Hook will now terminate the process.", "D3Hook", MB_OK | MB_ICONERROR);

		ExitProcess(EXIT_FAILURE);
	}

	return false;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			{
				debug("Initializing D3Hook...");

				// initialize module info
				if (!fxModule::Initialize(mymodule, hModule))
					LogFile::WriteLine("Couldn't initialize the module info!");

				AddVectoredExceptionHandler(1, HandleVariant);
				AddVectoredContinueHandler(1, CustomUnhandledExceptionFilter);

				// setup the current directory
				InitPath();

				LogFile::Initialize("d3hook.log", "--<< D3Hook log file >>--\n");

				bool configLoaded = InitConfigFile();

				LogFile::Format("Working directory is '%s'\n", gamedir);

				if (configLoaded) {
					LogFile::WriteLine("Configuration file was loaded successfully.");
				} else {
					LogFile::WriteLine("No configuration file was found.");
				}

				HMODULE hDI8Module = NULL;
				RSDSEntry gameInfo;

				if (IsGameSupported(gameInfo))
				{
					if (LoadSystemLibrary("dinput8.dll", &hDI8Module) &&
						GetHookProcAddress(hDI8Module, "DirectInput8Create", (FARPROC*)&lpDI8Create))
					{
						LogFile::WriteLine("Injected into the game process successfully.");

						// initialize the hook
						Initialize(gameInfo);
					}
					else
					{
						LogFile::WriteLine("Failed to inject into the game process.");
						MessageBox(NULL, "Could not inject into the game process. Unknown errors may occur.", "D3Hook", MB_OK | MB_ICONWARNING);
					}
				}
				else
				{
					LogFile::WriteLine("Unsupported game version! Terminating...");
					MessageBox(NULL, "Sorry, this version of Driv3r is unsupported. Please remove D3Hook to launch the game.", "D3Hook", MB_OK | MB_ICONERROR);

					ExitProcess(EXIT_FAILURE);
				}
			} break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}

	return true;
}
#elif defined(HOOK_EXE)
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

static void buildDirectoryStructure()
{
	auto create_if = [](const wchar_t* rDir)
	{
		auto total = makeToolPath(rDir);
		if (GetFileAttributesW(total.c_str()) == INVALID_FILE_ATTRIBUTES)
			CreateDirectoryW(total.c_str(), nullptr);
	};

	create_if(L"logs");
	create_if(L"mods");
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	debug("Initializing D3Hook...");

	LogFile::Initialize(makeToolPath(L"logs\\d3hook.log").c_str(), "--<< D3Hook log file >>--\n");

	bool configLoaded = InitConfigFile();

	buildDirectoryStructure();

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

	if (configLoaded) {
		LogFile::WriteLine("Configuration file was loaded successfully.");
	} else {
		LogFile::WriteLine("No configuration file was found.");
	}

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

	AddVectoredExceptionHandler(1, HandleVariant);
	AddVectoredContinueHandler(1, CustomUnhandledExceptionFilter);

	invokeEntry(entry);
	return 0;
}

#else
#error "Unknown platform!"
#endif