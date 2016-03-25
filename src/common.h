//
// Common resources
//

#pragma once

#define DIRECTINPUT_VERSION 0x0800

#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS

#define DLLEXPORT __declspec(dllexport)

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include <windowsx.h>

#include <ShlObj.h>

#include "util.h"
#include "logfile.h"