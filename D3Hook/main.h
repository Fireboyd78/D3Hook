#pragma once

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN 
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>

#include <d3d9.h>
#include <d3d9caps.h>
#include <d3d9helper.h>
#include <d3d9types.h>

#include "util.h"

#include "driv3r.h"
#include "IDirect3D9Hook.h"
#include "IDirect3DDevice9Hook.h"