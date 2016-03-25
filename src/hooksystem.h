#pragma once
#include "common.h"

typedef int(*LPFNINITIALIZEHOOK)(int);

struct HOOK_INIT_DATA {
    LPFNINITIALIZEHOOK fnInitialize;
    LPCSTR hookName;
};

namespace HookSystem {
    void Initialize(int gameVersion);
}