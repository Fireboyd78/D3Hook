#pragma once
#include "common.h"

#include "d3dhook.h"
#include "hook.h"

//
// Known addresses
//

#define D3ADDR_HWND                 0x7EC218
#define D3ADDR_IDIRECT3D9           0x8D7DEC
#define D3ADDR_IDIRECT3DDEVICE9     0x8D7DF0

// IMPORTANT!!!
// 0x8AC268 is related to message pumps
// [[[0x8AC268] + 0xC] + 0x7B4] is window_size_x ;)

class CDriv3r {
public:
    CDriv3r() {
        // nothing to initialize
    };

    ~CDriv3r() {};

    HWND GetMainWindowHwnd()                            { return *(HWND*)D3ADDR_HWND; };

    DWORD GetD3D()                                      { return *(DWORD*)D3ADDR_IDIRECT3D9; };
    DWORD GetD3DDevice()                                { return *(DWORD*)D3ADDR_IDIRECT3DDEVICE9; };

    void SetD3D(IDirect3D9 *pD3d)                       { *(DWORD*)D3ADDR_IDIRECT3D9 = *(DWORD*)pD3d; };
    void SetD3DDevice(IDirect3DDevice9 *pD3dDevice)     { *(DWORD*)D3ADDR_IDIRECT3DDEVICE9 = *(DWORD*)pD3dDevice; };
};