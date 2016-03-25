#include "driv3r.h"

D3_ADDR_DATA g_Offsets[] = {
    { HA_ADDR_HWND,
        {
            0x7F8340,
            0x7EC218
        }, "Hwnd"
    },
    { HA_ADDR_SINGLETON_OBJECTS,
        {
            0x8B8358,
            0x8AC230
        }, "SingletonObjects",
    },
    { HA_ADDR_VIEWPORT,
        {
            0x8DED88,
            0x8D7C98
        }, "HamsterViewport",
    },
    { HA_ADDR_IDIRECT3D9,
        {
            0x8DEEDC,
            0x8D7DEC
        }, "IDirect3D9",
    },
    { HA_ADDR_IDIRECT3DDEVICE9,
        {
            0x8DEEE0,
            0x8D7DF0
        }, "IDirect3DDevice9",
    },
};

CDriv3r::CDriv3r(int gameVersion)
    : CHamsterGame(gameVersion) {
    HA_ADDR_OFFSET_DATA offsetData;

    for (int i = 0; i < HA_ADDR_COUNT; i++)
        offsetData.offsets[i] = g_Offsets[i].GetAddressData(gameVersion);

    Initialize(offsetData);
};