#include "common.h"
#include "dbghelp.h"

#include <string>
#include <algorithm>

//
// Debugging utilities
//

void debug(const char *lpOutputStr) {
    OutputDebugStringA(lpOutputStr);
};

void debugf(const char *format, ...) {
    char buf[255];
    va_list va;
    va_start(va, format);
    _vsnprintf(buf, sizeof(buf), format, va);
    va_end(va);

    OutputDebugStringA(buf);
}

//
// Empty function templates
//

void ReturnVoid(void) {
    return;
};

int ReturnNullOrZero(void) {
    return 0;
};

bool ReturnFalse(void) {
    return false;
};

//
// Useful helper functions
//

bool LoadSystemLibrary(LPCSTR lpDllName, HMODULE *out)
{
    char szDllFile[MAX_PATH]{ NULL }; // path to system dll

    GetSystemDirectoryA(szDllFile, sizeof(szDllFile));
    sprintf(szDllFile, "%s\\%s", szDllFile, lpDllName);

    if (SUCCEEDED(*out = LoadLibraryA(szDllFile)))
    {
        LogFile::Format("Loaded system library \"%s\" => %08X\n", szDllFile, *out);
        return true;
    }
    else
    {
        LogFile::Format("System library \"%s\" not found!\n", szDllFile);
        return false;
    }
};

bool GetHookProcAddress(HMODULE hModule, LPCSTR lpProcName, FARPROC *out)
{
    if (SUCCEEDED(*out = GetProcAddress(hModule, lpProcName)))
    {
        LogFile::Format("Found hook proc \"%s\" in module %08X => %08X\n", lpProcName, hModule, *out);
        return true;
    }
    else
    {
        LogFile::Format("Hook proc \"%s\" not found in module %08X!\n", lpProcName, hModule);
        return false;
    }
};

bool GetPathSpec(char *path, char *dest, int destLen) {
    char ch;
    int idx = 0, len = 0;

    if ((path != NULL) && (dest != NULL))
    {
        while ((ch = path[idx++]) != NULL) {
            if (ch == '\\')
                len = idx;
        }

        if (len < destLen) {
            strncpy(dest, path, len);
            return true;
        }
    }

    return false;
}

constexpr const wchar_t* REG_DIR = L"SOFTWARE\\Fireboyd78\\d3hook";

class regStream
{
public:
	inline explicit regStream(const wchar_t *subKey, HKEY masterKey = HKEY_CURRENT_USER){
		RegOpenKeyExW(masterKey, subKey, 0, KEY_READ | KEY_WRITE, &key);
	}

	inline ~regStream() { close(); }
	inline bool good() const { return key; }

	inline void close() {
		if (key) {
			RegCloseKey(key);
			key = nullptr;
		}
	}

	inline void make(const wchar_t* subKey, HKEY masterKey = HKEY_CURRENT_USER) {
		RegCreateKeyW(masterKey, subKey, &key);
	}

	inline bool read(const wchar_t* name, uint8_t* data, size_t size) {
		if (key) {
			DWORD type = REG_SZ;
			DWORD rsize = static_cast<DWORD>(size);
			return RegQueryValueExW(key, name, nullptr, &type, data, &rsize /*size in bytes*/);
		}
		return false;
	}

	inline bool write(const wchar_t* name, const uint8_t* data, size_t size) {
		if (key) {
			return RegSetValueExW(key, name, 0, REG_EXPAND_SZ, data, size /*size in bytes*/);
		}
		return false;
	}

	template<typename T>
	inline bool read(const wchar_t *name, T& val) {
		return read(name, reinterpret_cast<uint8_t*>(&val), sizeof(val));
	}

	template<typename T>
	bool write(const wchar_t* name, const T& val) {
		return write(name, reinterpret_cast<const uint8_t*>(&val), sizeof(val));
	}

	template<typename T>
	inline bool write(const wchar_t* name, const std::basic_string<T> &str) {
		return write(name, reinterpret_cast<const uint8_t*>(str.c_str()), str.size() * sizeof(T) /*wchar compat*/);
	}

	/*user must prepare string*/
	template<typename T>
	inline bool read(const wchar_t* name, std::basic_string<T>& str) {
		return read(name, reinterpret_cast<uint8_t*>(str.data()), str.size() * sizeof(T));
	}

private:
	HKEY key = nullptr;
};

std::wstring selectGamePath()
{
	wchar_t gameDir[MAX_PATH]{};

	// user requests path change
	bool reqReselect = GetAsyncKeyState(VK_SHIFT) & 0x8000;

	regStream req(REG_DIR);
	if (req.good() && !reqReselect)
		req.read(L"gameBin", gameDir);
	else {
		req.make(REG_DIR);

		OPENFILENAMEW file{};

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
			MessageBoxW(nullptr, L"Failed to retrieve the game path to the executable. Cannot launch D3Hook!", L"D3Hook", MB_OK);
			TerminateProcess(GetCurrentProcess(), 0);
		}

		std::wstring str = path;
		std::replace(str.begin(), str.end(), '\\', '/');

		str = str.substr(0, str.find_last_of(L"/") + 1);

		req.write(L"gameBin", str);
		return str;
	}

	return gameDir;
}

std::wstring makeGamePath(const std::wstring& rel)
{
    static std::wstring gameRoot;
    if (gameRoot.empty()) {
		gameRoot = selectGamePath();
    }

	std::wstring newPath = gameRoot + rel;

	// sanitize the path (this is a windows thing, on linux & osx we
	// need to do it the other way around)
	std::replace(newPath.begin(), newPath.end(), L'/', L'\\');

	return newPath;
}

std::wstring makeToolPath(const std::wstring& rel_to)
{
	static wchar_t exePath[MAX_PATH]{};

	if (!exePath[0]) {
		wchar_t buf[MAX_PATH];
		GetModuleFileNameW((HMODULE)nullptr, buf, MAX_PATH);
		_wsplitpath(buf, &exePath[0], &exePath[_MAX_DRIVE - 1], nullptr, nullptr);
	}

	std::wstring newPath = exePath + rel_to;

	// sanitize the path
	std::replace(newPath.begin(), newPath.end(), L'/', L'\\');
	
	return newPath;
}