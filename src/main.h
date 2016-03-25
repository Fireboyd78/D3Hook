#pragma once

#include "common.h"

#include "driv3r.h"

#include "hook.h"
#include "hooksystem.h"

// dinput hooking
#include <dinput.h>

typedef HRESULT(WINAPI *LPFNDIRECTINPUT8CREATE)(HINSTANCE, DWORD, REFIID, LPVOID *, LPUNKNOWN);

EXTERN_C LPFNDIRECTINPUT8CREATE lpDI8Create;
EXTERN_C HRESULT DirectInput8Create_Impl(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);