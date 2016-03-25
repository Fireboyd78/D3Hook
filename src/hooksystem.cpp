#include "hooksystem.h"
#include "driv3r.h"

bool g_hookSystemInitialzed = false;

const HOOK_INIT_DATA g_hookInitTable[] = {
    { &FileChunkerHookSystem::Initialize, "FileChunkerHookSystem" },
};

void HookSystem::Initialize(int gameVersion) {
    LOG("*** HookSystem::Initialize ***");

    if (g_hookSystemInitialzed)
    {
        LOG(" - The hook system is already initialized!");
        return;
    }

    for (HOOK_INIT_DATA hookInit : g_hookInitTable)
    {
        int status = hookInit.fnInitialize(gameVersion);

        switch (status) {
            case HOOK_INIT_OK:
                LogFile::Format(" - Successfully initialized \"%s\"!\n", hookInit.hookName);
                break;
            case HOOK_INIT_FAILED:
                LogFile::Format(" - Failed to initialize \"%s\"!\n", hookInit.hookName);
                break;
            case HOOK_INIT_UNSUPPORTED:
                LogFile::Format(" - Cannot initialize \"%s\" due to an unsupported game version!\n", hookInit.hookName);
                break;
            default:
                LogFile::Format(" - Initializing \"%s\" returned an unknown status: %d\n", hookInit.hookName, status);
                break;
        }
    }
};