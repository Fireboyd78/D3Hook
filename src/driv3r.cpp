#include "driv3r.h"

addr_info g_singleton_objs { { 0x8B8358, 0x8AC230 }, "SingletonObjects" };
addr_info g_addrs[HA_ADDR_COUNT];

static int g_GameVersion;

intptr_t addressof(addr_info &info) {
    LogFile::Format("addressof gameversion %d\n", g_GameVersion);
    return info.offsets[g_GameVersion];
}

namespace hamster {
    intptr_t GetPointer(HA_ADDR_TYPE type) {
        return *(intptr_t*)g_addrs[type].offsets[g_GameVersion];
    }

    void SetPointer(HA_ADDR_TYPE type, intptr_t value) {
        *(intptr_t*)g_addrs[type].offsets[g_GameVersion] = value;
    };

    intptr_t * GetSingletonObjectsPointer(void) {
        return reinterpret_cast<intptr_t *>(g_singleton_objs.offsets[g_GameVersion]);
    }
}

CDriv3r::CDriv3r(int gameVersion) {
    g_GameVersion = gameVersion;

    LogFile::WriteLine("Initializing static offsets...");

    g_addrs[HA_ADDR_WINDOW]             = { { 0x7F8340, 0x7EC218 }, "Window" };
    g_addrs[HA_ADDR_VIEWPORT]           = { { 0x8DED88, 0x8D7C98 }, "HamsterViewport" };
    g_addrs[HA_ADDR_DIRECT3D]           = { { 0x8DEEDC, 0x8D7DEC }, "IDirect3D9" };
    g_addrs[HA_ADDR_DIRECT3D_DEVICE]    = { { 0x8DEEE0, 0x8D7DF0 }, "IDirect3DDevice9", };

    for (auto addr : g_addrs) {
        LogFile::Format(" - '%s' => %08X\n", addr.name, addr.offsets[g_GameVersion]);
    }

    // temp. hack cuz lazy
    //for (int i = 0; i < SOBJ_COUNT; i++)
    //    LogFile::Format("D3_SOBJ[%04d] -> %04X\n", i, (((i * 4) + i) * 4));
};

int CDriv3r::Version() {
    return g_GameVersion;
}