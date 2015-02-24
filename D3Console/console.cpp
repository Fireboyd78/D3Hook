#include "console.h"

using namespace std;

HMODULE hModule = 0;
HOOKPROC hProc  = 0;
HHOOK hHook     = 0;

BOOL APIENTRY WndProcHook(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved);

int main(int argc, _TCHAR* argv[])
{
    //const char *dir = "C:\\Dev\\Research\\Driv3r\\";
    //char file[255];
    //
    //sprintf(file, "%sDriv3r.exe", dir);
    //
    //int err = (int)ShellExecute(NULL, "open", file, NULL, dir, SW_HIDE);
    //
    //if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND)
    //    MessageBox(NULL, "Failed to open Driv3r!", "Epic fail!", MB_ICONERROR);

    char cmd[64];

    do
    {
        fflush(stdout);
        scanf("%s:%n", cmd);
        MessageBox(NULL, cmd, "D3Hook", MB_ICONINFORMATION);
    } while (true);

    MessageBox(NULL, "Bye!", "D3Hook", MB_ICONINFORMATION);
	return 0;
}

EXTERN_C __declspec(dllexport) LRESULT CALLBACK WndProcHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

