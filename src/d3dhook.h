#pragma once

//
// Resources for hooking into Direct3D
//

#include <d3d9.h>
#include <d3d9caps.h>
#include <d3d9helper.h>
#include <d3d9types.h>

#include <d3dx9.h>

#include "common.h"

extern IDirect3D9          *pD3D;
extern IDirect3DDevice9    *pD3DDevice;

#include <IDirect3D9Hook.h>
#include <IDirect3DDevice9Hook.h>