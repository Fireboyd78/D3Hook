#include "driv3r.h"

const GUID _GUID_DRIV3R_V100 = { 0x3D0F8640, 0xCCFA, 0x4771, 0xA9, 0x16, 0x24, 0xFB, 0xA2, 0xD7, 0xA1, 0xD1 }; // GUID for Driv3r V1.0
const GUID _GUID_DRIV3R_V120 = { 0xCA618E9C, 0x78BD, 0x4621, 0xB0, 0xC1, 0x45, 0x4F, 0xAA, 0xBE, 0x8D, 0xF3 }; // GUID for Driv3r V1.2
const GUID _GUID_DRIV3R_DEMO = { 0xC8EB0F56, 0x4B15, 0x1000, 0x85, 0x71, 0xD6, 0x9C, 0x32, 0xF1, 0xB8, 0xBC }; // GUID for Driv3r (Demo)

const RSDSEntry g_rsds_info[D3_NUM_DEFINED_VERSIONS] = {
    { 0x791A74, __DRIV3R_V100, 1.00f, true, _GUID_DRIV3R_V100, 1, "Z:\\Projects\\Driver3\\Dev\\Driver3\\Win32\\Master\\Driver3.pdb" },
    { 0x78F324, __DRIV3R_V120, 1.20f, true, _GUID_DRIV3R_V120, 1, "z:\\Projects\\Driver3\\Dev\\Driver3\\Win32\\Master\\Driver3.pdb" },

    { 0x785224, __DRIV3R_DEMO, 1.00f, false, _GUID_DRIV3R_DEMO, 1, "Z:\\Projects\\Driver3\\Dev\\Driver3\\Win32\\Demo_Master\\Driver3.pdb" },
};


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
}

void CDriv3r::Initialize() {
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
}

bool CDriv3r::GetGameInfo(RSDSEntry & ppGameInfo) {
    for (auto &entry : g_rsds_info)
    {
        auto rsds = (LPRSDS_DATA)(DWORD*)entry.offset;

        if (IS_RSDS_VALID(rsds) && IS_RSDS_DATA_EQUAL(rsds, entry.guid, entry.age, entry.filename))
        {
            ppGameInfo = entry;
            return true;
        }
    }
    return false;
}

int CDriv3r::Version() {
    return g_GameVersion;
}