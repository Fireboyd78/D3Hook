#pragma once
#ifndef __HAMSTER_GAME_HEADER_INCLUDED
#define __HAMSTER_GAME_HEADER_INCLUDED
#include "hamster.h"
#else
#error Cannot include more than one Hamster-based game header!
#endif

#include "CFileChunker.h"
#include "models.h"

#define __DRIV3R_V100 0     // Vanilla
#define __DRIV3R_V120 1     // Patch 2
#define __DRIV3R_DEMO 16    // Demo

//
// Number of defined game versions
//
#define D3_NUM_DEFINED_VERSIONS     3

//
// Number of supported game versions
//
#define D3_NUM_SUPPORTED_VERSIONS   2

const GUID _GUID_DRIV3R_V100 = { 0x3D0F8640, 0xCCFA, 0x4771, 0xA9, 0x16, 0x24, 0xFB, 0xA2, 0xD7, 0xA1, 0xD1 }; // GUID for Driv3r V1.0
const GUID _GUID_DRIV3R_V120 = { 0xCA618E9C, 0x78BD, 0x4621, 0xB0, 0xC1, 0x45, 0x4F, 0xAA, 0xBE, 0x8D, 0xF3 }; // GUID for Driv3r V1.2
const GUID _GUID_DRIV3R_DEMO = { 0xC8EB0F56, 0x4B15, 0x1000, 0x85, 0x71, 0xD6, 0x9C, 0x32, 0xF1, 0xB8, 0xBC }; // GUID for Driv3r (Demo)

const RSDSEntry g_rsds_info[D3_NUM_DEFINED_VERSIONS] = {
    { 0x791A74, __DRIV3R_V100, 1.00, true, _GUID_DRIV3R_V100, 1, "Z:\\Projects\\Driver3\\Dev\\Driver3\\Win32\\Master\\Driver3.pdb" },
    { 0x78F324, __DRIV3R_V120, 1.20, true, _GUID_DRIV3R_V120, 1, "z:\\Projects\\Driver3\\Dev\\Driver3\\Win32\\Master\\Driver3.pdb" },

    { 0x785224, __DRIV3R_DEMO, 1.00, false, _GUID_DRIV3R_DEMO, 1, "Z:\\Projects\\Driver3\\Dev\\Driver3\\Win32\\Demo_Master\\Driver3.pdb" },
};

struct addr_info {
    intptr_t offsets[D3_NUM_SUPPORTED_VERSIONS];
    const char *name;
};

intptr_t addressof(addr_info &info);

/*
Dear Patch 2:

    YOU SUCK.
    
    A lot.
    
    Fuck you.
*/
#define HANDLER_CANNOT_BE_IMPLEMENTED_BECAUSE_PATCH_2_SUCKS() \
    LogFile::WriteLine(" - Sorry, Patch 2 is too much of a fucking mess for this.")

/* Singleton object types */
enum HA_SOBJ_TYPE {
    SOBJ_FILESYSTEM                         = 0,
    SOBJ_RENDERER                           = 2,
    SOBJ_OCCLUDER_MANAGER                   = 5,
    SOBJ_SYSTEMCONFIG                       = 6,
    SOBJ_TEXTMANAGER                        = 8,
    SOBJ_FONTMANAGER                        = 9,

    SOBJ_FRAMEWORK                          = 10,

    SOBJ_USERCOMMANDPROXY                   = 14,
    SOBJ_MOUSECONTROL                       = 16,
    
    SOBJ_SOUND                              = 32,
    SOBJ_MUSIC                              = 33,
    SOBJ_LIFE_NODE_COLLECTION               = 39,
    
    SOBJ_STRING_COLLECTION                  = 40,
    SOBJ_ACTIVE_NODE_COLLECTION             = 41,
    SOBJ_WIRE_COLLECTION_MANAGER            = 42,
    SOBJ_LIFE_NODE_PROPERTY_STREAM          = 43,
    SOBJ_SCRIPT_COUNTER_COLLECTION          = 44,
    SOBJ_ACTIVE_NODE_COLLECTION_MANAGER     = 46,
    SOBJ_LIFE_NODE_FACTORY                  = 47,
    SOBJ_LIFE_ACTOR_FACTORY                 = 48,
    SOBJ_LIFE_ACTOR_PROPERTY_STREAM         = 49,
    
    SOBJ_LIFE_ACTOR_COLLECTION              = 50,
    SOBJ_SOUNDBANK_COLLECTION               = 51,

    SOBJ_GAME                               = 55,
    
    SOBJ_VEHICLE_SPEC_MANAGER               = 80,
    SOBJ_VO3MANAGER                         = 81,
    SOBJ_DRIVE_TYPE_LOOKUP                  = 89,
    
    SOBJ_CITY_SPOOL_BUFFER                  = 94,
    SOBJ_INITIALISATION_DATA                = 95,
    SOBJ_MENU_MANAGER                       = 96,
    SOBJ_GAMEMENU_LINK                      = 97,
    SOBJ_MENU_SOUND                         = 98,

    SOBJ_VEHICLE_MANAGER                    = 139,

    SOBJ_VEHICLE_SOUND_SPEC_MANAGER         = 153,

    SOBJ_PROFILE_SETTINGS                   = 215,
    SOBJ_FRONTEND_MUSIC                     = 217,
    SOBJ_GENERIC_STRING_MANAGER             = 218,

    /* Number of singleton objects */
    SOBJ_COUNT                              = 230
};

/*
//
// Known addresses
//
#if D3_VERSION == __DRIV3R_V100
    #define D3ADDR_HWND                 0x7F8340
    #define D3ADDR_USERCOMMANDPROXY     0x8B8390
    #define D3ADDR_MOUSECONTROL         0x8B8398
    
    #define D3ADDR_VIEWPORT             0x8DED88
    #define D3ADDR_IDIRECT3D9           0x8DEEDC // Viewport + 0x154
    #define D3ADDR_IDIRECT3DDEVICE9     0x8DEEE0 // Viewport + 0x158
#elif D3_VERSION == __DRIV3R_V120
    #define D3ADDR_HWND                 0x7EC218
    #define D3ADDR_USERCOMMANDPROXY     0x8AC268
    #define D3ADDR_MOUSECONTROL         0x8AC270
    
    #define D3ADDR_VIEWPORT             0x8D7C98
    #define D3ADDR_IDIRECT3D9           0x8D7DEC
    #define D3ADDR_IDIRECT3DDEVICE9     0x8D7DF0
#endif
*/

class CDriv3r {
public:
    CDriv3r(int gameVersion);

    static bool GetGameInfo(RSDSEntry &ppGameInfo) {
        for (auto entry : g_rsds_info)
        {
            auto rsds = (LPRSDS_DATA)(DWORD*)entry.offset;

            if (IS_RSDS_VALID(rsds) && IS_RSDS_DATA_EQUAL(rsds, entry.guid, entry.age, entry.filename))
            {
                ppGameInfo = entry;
                return true;
            }
        }
        return false;
    };

    static int Version();

    HWND GetMainWindow(void)                                    { return hamster::GetPointer<HWND>(HA_ADDR_WINDOW); };

    IDirect3D9* GetD3D(void)                                    { return hamster::GetPointer<IDirect3D9*>(HA_ADDR_DIRECT3D); };
    void SetD3D(IDirect3D9 *pD3d)                               { return hamster::SetPointer(HA_ADDR_DIRECT3D, pD3d); };

    IDirect3DDevice9* GetD3DDevice(void)                        { return hamster::GetPointer<IDirect3DDevice9*>(HA_ADDR_DIRECT3D_DEVICE); };
    void SetD3DDevice(IDirect3DDevice9 *pD3dDevice)             { hamster::SetPointer(HA_ADDR_DIRECT3D_DEVICE, pD3dDevice); };

    IUserCommandProxy* GetUserCommandProxy(void)                { return hamster::GetSingletonObject<IUserCommandProxy*>(SOBJ_USERCOMMANDPROXY); };
    void SetUserCommandProxy(IUserCommandProxy *pUserCmdProxy)  { hamster::SetSingletonObject(SOBJ_USERCOMMANDPROXY, pUserCmdProxy); };
};