//
// Common resources
//

#pragma once

#define DIRECTINPUT_VERSION 0x0800

#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS
#define _ENABLE_EXTENDED_ALIGNED_STORAGE

#define DLLEXPORT __declspec(dllexport)

#include <cassert>
#include <cstdint>

#include <type_traits>

#include <fstream>
#include <vector>

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include <windowsx.h>

#include <Shlwapi.h>
#include <ShlObj.h>

#include <utility>

#ifdef HOOK_EXE
using filename_t = LPCWSTR;
#else
using filename_t = LPCSTR;
#endif

#include "settings.h"

#include "util.h"
#include "logfile.h"
#include "console.h"

//  C4091: '__declspec(novtable)': ignored on left of 'x' when no variable is declared
#pragma warning (disable : 4091)
// disable warning/error about __identifier(<string>)
#pragma warning(disable:4483)