#include "common.h"

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