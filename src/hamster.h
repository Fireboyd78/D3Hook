#pragma once
#include "common.h"

#include "d3dhook.h"
#include "hook.h"

//
// Hamster shared resources
//

#include "types.h"
#include "FileChunker.h"
#include "IUserCommandProxy.h"

enum HA_ADDR_TYPE {
    HA_ADDR_HWND,

    HA_ADDR_SINGLETON_OBJECTS, // list of singleton objects

    HA_ADDR_VIEWPORT,

    HA_ADDR_IDIRECT3D9,
    HA_ADDR_IDIRECT3DDEVICE9,

    /**** END OF DATA ****/
    HA_ADDR_COUNT
};

struct HA_SOBJ_DATA {
    BYTE type;
    const char *name;
};

struct HA_ADDR_DATA {
    HA_ADDR_TYPE type;
    DWORD offset;
    LPCSTR name;
};

struct HA_ADDR_OFFSET_DATA {
    HA_ADDR_DATA offsets[HA_ADDR_COUNT];
};

class CHamsterGame {
protected:
    DWORD m_offsets[HA_ADDR_COUNT];
    DWORD *m_sobjs_offset;

    int m_gameVersion;

    NOINLINE void Initialize(HA_ADDR_OFFSET_DATA &offsetData) {
        LogFile::WriteLine("Resolving static offsets...");
        for (auto addr : offsetData.offsets)
        {
            DWORD offset = addr.offset;
            m_offsets[addr.type] = offset;

            LogFile::Format(" - '%s' => %08X\n", addr.name, offset);
        }
        LogFile::WriteLine("...Done");

        // used to resolve singleton offsets
        m_sobjs_offset = (DWORD*)m_offsets[HA_ADDR_SINGLETON_OBJECTS];

        // temp. hack cuz lazy
        //for (int i = 0; i < D3_SOBJ_COUNT; i++)
        //    LogFile::Format("D3_SOBJ[%04d] -> %04X\n", i, (((i * 4) + i) * 4));
    };

public:
    CHamsterGame(int gameVersion) {
        m_gameVersion = gameVersion;
    };

    NO_INL int GetGameVersion() const {
        return m_gameVersion;
    };

    NOINLINE DWORD GetPointer(HA_ADDR_TYPE type) {
        return *(DWORD*)m_offsets[type];
    };

    NOINLINE bool SetPointer(HA_ADDR_TYPE type, LPVOID value) {
        return ((LPVOID)(*(DWORD*)m_offsets[type] = (DWORD)value) == value);
    };

    NOINLINE DWORD GetSingletonPointer(BYTE type) {
        return *(DWORD*)&m_sobjs_offset[type];
    };

    NOINLINE bool SetSingletonPointer(BYTE type, LPVOID value) {
        return ((LPVOID)(*(DWORD*)&m_sobjs_offset[type] = (DWORD)value) == value);
    };

    template<typename T>
    T GetPointer(HA_ADDR_TYPE type) {
        return (T)GetPointer(type);
    };

    template<typename T>
    T GetSingletonPointer(BYTE type) {
        return (T)GetSingletonPointer(type);
    };

    NO_INL HWND GetMainWindowHwnd(void)                                 { return GetPointer<HWND>(HA_ADDR_HWND); };

    NO_INL IDirect3D9* GetD3D(void)                                     { return GetPointer<IDirect3D9*>(HA_ADDR_IDIRECT3D9); };
    NO_INL IDirect3DDevice9* GetD3DDevice(void)                         { return GetPointer<IDirect3DDevice9*>(HA_ADDR_IDIRECT3DDEVICE9); };

    NO_INL bool SetD3D(IDirect3D9 *pD3d)                                { return SetPointer(HA_ADDR_IDIRECT3D9, pD3d); };
    NO_INL bool SetD3DDevice(IDirect3DDevice9 *pD3dDevice)              { return SetPointer(HA_ADDR_IDIRECT3DDEVICE9, pD3dDevice); };
};
