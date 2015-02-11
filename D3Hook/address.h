#pragma once
#include "util.h"

//
// Known addresses
//

#define D3ADDR_HWND                 0x7EC218
#define D3ADDR_IDIRECT3D9           0x8D7DEC
#define D3ADDR_IDIRECT3DDEVICE9     0x8D7DF0

// IMPORTANT!!!
// 0x8AC268 is related to message pumps
// [[[0x8AC268] + 0xC] + 0x7B4] is window_size_x ;)