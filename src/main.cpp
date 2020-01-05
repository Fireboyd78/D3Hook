
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
			__debugbreak();
			LogFile::Format(" - Detected game version %1.2f\n", gameInfo.fancy_version);

			Initialize(gameInfo);
			initGuard = true;
		}
	}
}

std::wstring make_abs_path(const std::wstring& rel_to)
{
	static wchar_t executable_path[MAX_PATH] = { '\0' };

	if (executable_path[0] == '\0') {
		wchar_t buf[MAX_PATH];
		GetModuleFileNameW((HMODULE)nullptr, buf, MAX_PATH);
		_wsplitpath(buf, &executable_path[0], &executable_path[_MAX_DRIVE - 1], nullptr, nullptr);
	}

	wchar_t buf[MAX_PATH];
	lstrcpyW(buf, executable_path);
	lstrcatW(buf, rel_to.c_str());

	wchar_t final_buf[MAX_PATH] = { '\0' };
	PathCanonicalizeW(final_buf, buf);

	return final_buf;
}

static std::wstring g_gameRoot;

//TODO: make this not shit?
// workaround for "bug" at + 005A6937
const char* WINAPI GetCommandLineA_Fake()
{
	static std::string poop;

	if (poop.empty()) {
		using convert_type = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;
		poop = converter.to_bytes(g_gameRoot) + "\\notfake.exe";
	}

	return poop.c_str();
}

std::wstring GetGamePath()
{
	HKEY hKey;
	DWORD dwType = REG_SZ, size = MAX_PATH;
	wchar_t inst_dir[MAX_PATH] = { 0 };

	if (RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Fireboyd78\\d3hook", NULL, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		RegQueryValueExW(
			hKey, L"gameBin", nullptr, &dwType, reinterpret_cast<LPBYTE>(inst_dir), &size);
		RegCloseKey(hKey);
	}
	else
	{
		OPENFILENAMEW file;
		ZeroMemory(&file, sizeof(file));

		wchar_t path[MAX_PATH] = { 0 };
		file.lStructSize = sizeof(file);
		file.nMaxFile = MAX_PATH;
		file.lpstrFilter = L"Executables\0*.exe\0";
		file.lpstrDefExt = L"EXE";
		file.lpstrTitle = L"Please select your Driver executable (Driv3r.exe)";
		file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER;

		file.lpstrFile = path;
		file.lpstrInitialDir = path;

		auto hr = GetOpenFileNameW(&file);
		if (!hr)
		{
			MessageBoxW(nullptr, L"Failed to retrieve gamepath and game-executable. Cannot launch D3Hook!", L"D3Hook", MB_OK);
			TerminateProcess(GetCurrentProcess(), 0);
		}

		std::wstring str = path;
		std::replace(str.begin(), str.end(), '\\', '/');

		str = str.substr(0, str.find_last_of(L"/") + 1);

		//read the file's headers to verify its x86
		bool isCorrectMachineType = false;
		const HANDLE h_mapping = INVALID_HANDLE_VALUE;
		const HANDLE h_file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
			FILE_ATTRIBUTE_READONLY, nullptr);
		if (h_file != INVALID_HANDLE_VALUE)
		{
			const HANDLE h_mapping = CreateFileMapping(h_file, nullptr, PAGE_READONLY | SEC_IMAGE, 0, 0, nullptr);
			if (h_mapping != INVALID_HANDLE_VALUE)
			{
				const LPVOID addr_header = MapViewOfFile(h_mapping, FILE_MAP_READ, 0, 0, 0);
				if (addr_header != nullptr)
				{
					const PIMAGE_NT_HEADERS pe_hdr = ImageNtHeader(addr_header);
					if (pe_hdr != nullptr)
					{
						isCorrectMachineType = (pe_hdr->FileHeader.Machine == IMAGE_FILE_MACHINE_I386);
					}
				}
			}
		}

		if (h_file != INVALID_HANDLE_VALUE)
			CloseHandle(h_file);

		if (h_mapping != INVALID_HANDLE_VALUE)
			CloseHandle(h_mapping);

		if (!isCorrectMachineType)
		{
			MessageBoxA(nullptr, "The selected executable is a non x86 bit executable! Please reselect...", "SAP Exception",
				MB_OK);
			TerminateProcess(GetCurrentProcess(), 0);
		}

		RegCreateKeyW(HKEY_CURRENT_USER, L"SOFTWARE\\Fireboyd78\\d3hook", &hKey);
		RegSetValueExW(hKey, L"gameBin", NULL, REG_EXPAND_SZ, reinterpret_cast<LPBYTE>(const_cast<wchar_t*>(str.c_str())),
			static_cast<DWORD>(str.length() + 1) * sizeof(TCHAR));
		RegCloseKey(hKey);

		return str;
	}

	return std::wstring(inst_dir);
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

	auto gamePath = GetGamePath();
	g_gameRoot = gamePath;

	auto* hkernel = GetModuleHandleW(L"kernel32.dll");
	auto _AddDllDirectory = (decltype(&AddDllDirectory))GetProcAddress(hkernel, "AddDllDirectory");
	auto _SetDefaultDllDirectories = (decltype(&SetDefaultDllDirectories))GetProcAddress(hkernel, "SetDefaultDllDirectories");

	/*w 8+ only*/
	if (_AddDllDirectory && _SetDefaultDllDirectories) {

		_SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_USER_DIRS);
		_AddDllDirectory(gamePath.c_str());	
		_AddDllDirectory(make_abs_path(L"").c_str());
	}

	SetCurrentDirectoryW(gamePath.c_str());
	SetDllDirectoryW(gamePath.c_str());

	LogFile::Format("Game directory is '%S'\n", gamePath.c_str());

	FILE* game_file = nullptr;
	_wfopen_s(&game_file, (gamePath + L"Driv3r.exe").c_str(), L"rb");

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