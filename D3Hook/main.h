#pragma once

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN 
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdarg.h>
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

struct IVTableHook
{    
private:
    DWORD *m_address;
public:
    IVTableHook(DWORD dwAddress) { m_address = _PTR(dwAddress); }
    
    ~IVTableHook() {}

    operator LPVOID () {
        return (LPVOID)m_address;
    }

    const LPVOID GetMemberAddress(DWORD dwOffset) {
        return (LPVOID)(*m_address + dwOffset);
    };
};