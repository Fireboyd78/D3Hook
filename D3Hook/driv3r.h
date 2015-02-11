#pragma once

#include "address.h"
#include "main.h"

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