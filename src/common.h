//
// Common resources
//

#pragma once

#define DIRECTINPUT_VERSION 0x0800

#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS

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

#include "util.h"
#include "logfile.h"

//  C4091: '__declspec(novtable)': ignored on left of 'x' when no variable is declared
#pragma warning (disable : 4091)
// disable warning/error about __identifier(<string>)
#pragma warning(disable:4483)