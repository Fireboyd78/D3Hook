#pragma once

#include "common.h"
#include "patch.h"

#include "driv3r.h"

#include "hook.h"
#include "hooksystem.h"

#include "fiero.h"

// dinput hooking
#include <dinput.h>

typedef HRESULT(WINAPI *LPFNDIRECTINPUT8CREATE)(HINSTANCE, DWORD, REFIID, LPVOID *, LPUNKNOWN);

EXTERN_C LPFNDIRECTINPUT8CREATE lpDI8Create;
EXTERN_C HRESULT WINAPI DirectInput8Create_Impl(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);