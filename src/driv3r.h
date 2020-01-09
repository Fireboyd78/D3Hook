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

class IFramework PASS;

namespace hamster
{
    /*
        Singleton object types

          Some of these are spot-on, others are just best-guesses.
          Also, it's a /nightmare/ to look at.
        
          At least someone re-organized them for Driver: Parallel Lines :)

        Unknowns:
         - 21,
         - 91-93,
         - 101,
         - 112, 116, 117, 119-121, 125, 127, 132,
         - 164,
         - 184,
         - 190, 193-195, 199,
         - 202, 224-226
    */
    enum class ESingletonType : int
    {
        FileSystem                      = 0,
        RendererHandler                 = 1,
        Renderer                        = 2,
        DebugModel                      = 3,
        RuntimeModelManager             = 4,
        OccluderManager                 = 5,
        SystemConfig                    = 6,
        TaskManager                     = 7,
        TextManager                     = 8,
        FontManager                     = 9,
        Framework                       = 10,
        Loop                            = 12,
        HamsterInitialization           = 13,
        UserCommandProxy                = 14,
        MainMenu                        = 15,
        MouseControl                    = 16,
        StatusText                      = 17,
        OSControl                       = 18,
        Win32Wrapper                    = 19,

        AnimClipsManager                = 20,
        AnimFile                        = 27,
        BinaryKeyFrameLoader            = 29,
        CharacterDataManager            = 30,
        SoundSystem                     = 31,
        Sound                           = 32,
        Music                           = 33,
        Gamepad                         = 34,
        MemoryUnit                      = 35,
        LifeEventDataManager            = 37,
        LifeSystemCommentLog            = 38,
        LifeNodeCollection              = 39,
        StringCollection                = 40,
        ActiveNodeCollection            = 41,
        WireCollectionManager           = 42,
        LifeNodePropertyStream          = 43,
        ScriptCounterCollection         = 44,
        ActiveNodeCollectionManager     = 46,
        LifeNodeFactory                 = 47,
        LifeActorFactory                = 48,
        LifeActorPropertyStream         = 49,
        LifeActorCollection             = 50,
        SoundBankCollection             = 51,
        Physics                         = 52,
        RoadData                        = 53,
        Game                            = 55,
        GameRepository                  = 56,
        AIBehaviourManager              = 57,
        AIManager                       = 58,
        AICopCarManager                 = 59,
        AIFelonySystemManager           = 60,
        AICivilianCarManager            = 61,
        AICivilianRoadMap               = 62,
        MonorailManager                 = 63,
        TramManager                     = 64,
        AnimSystem                      = 65,
        DataManager                     = 66,
        SpoolDataFactory                = 67,
        InstanceDataManager             = 68,
        TerrainDataManager              = 69,
        SuperRegionPhysicsDataManager   = 70,
        RegionPhysicsDataManager        = 71,
        RegionRoutefindManager          = 72,
        AnimatedObjectDataManager       = 73,
        AttractorRegionDataManager      = 74,
        InteriorSceneDataReceiver       = 75,
        LightingEnvironment             = 76,
        InteriorInstanceManager         = 77,
        SceneDataReceiver               = 78,
        SceneDataManager                = 79,
        VehicleSpecManager              = 80,
        VO3Server                       = 81,
        PropDataManager                 = 82,
        WaterManager                    = 84,
        WaterDataManager                = 85,
        RegionWaterDataManager          = 86,
        LifeEventData                   = 87,
        LifeSceneData                   = 88,
        DriveTypeLookup                 = 89,
        Personalities                   = 90,
        CitySpoolBuffer                 = 94,
        InitialisationData              = 95,
        MenuManager                     = 96,
        MenuLink                        = 97,
        MenuSound                       = 98,
        MenuMovie                       = 99,
        LifeSystem                      = 102,
        LifeSystemSpoolCentre           = 103,
        ExtraCreationDataManager        = 104,
        LifeVisuals                     = 105,
        Environment                     = 106,
        CharacterTargetManager          = 107,
        CharacterCategoryManager        = 108,
        LifeSystemChaseTargets          = 111,
        TargetManager                   = 113,
        LifeEventCreationData           = 114,
        LifeAcquirableVehicleManager    = 118,
        TargetWatchList                 = 122,
        CollisionWatchList              = 123,
        ParticleFxGFXMemoryManager      = 126,
        SimulationWrapper               = 128,
        GameTime                        = 129,
        AnimatedObjectManager           = 130,
        GameState                       = 133,
        TerrainSearch                   = 134,
        StaticInstanceManager           = 135,
        FragmentManager                 = 136,
        PropManager                     = 137,
        InputManager                    = 138,
        VehicleManager                  = 139,
        ResidentArea                    = 140,
        GameSoundManager                = 141,
        VehicleSoundManager             = 142,
        CharacterSoundManager           = 143,
        ObjectSoundManager              = 144,
        CollisionSoundManager           = 145,
        GunSoundManager                 = 146,
        WheelSoundManager               = 147,
        AmbientSoundManager             = 148,
        ReverbSoundManager              = 149,
        CopSoundManager                 = 150,
        CDPlayer                        = 151,
        MusicSystem                     = 152,
        VehicleSoundSpecManager         = 153,
        WeaponsStore                    = 154,
        KillKillKill                    = 155,
        SFXServer                       = 156,
        VehicleAcquisition              = 157,
        MovingObjectManager             = 158,
        Rumble                          = 159,
        CharacterManager                = 160,
        AnimationLookup                 = 161,
        SpoolManager                    = 162,
        GenericViewport                 = 165,
        GameOverlayManager              = 166,
        State_Main                      = 167,
        State_Controller                = 168,
        State_Intro                     = 169,
        State_GameAssets                = 170,
        State_Frontend                  = 173,
        State_FrontendLoader            = 174,
        State_ChooseCity                = 176,
        State_MULoadReplay              = 177,
        State_MULoadSaveProfile         = 178,
        LifeProgression                 = 179,
        State_Game                      = 180,
        LoadState                       = 182,
        State_GameRunning               = 185,
        State_GameStalled               = 186,
        State_FilmDirector              = 188,
        State_PauseMenu                 = 194,
        State_MUAutoSave                = 201,
        State_Reload                    = 204,
        State_Release                   = 207,
        State_Movie                     = 209,
        ControllerStateTree             = 210,
        InGameMovie                     = 211,
        LoadingScreen                   = 213,
        WipeManager                     = 214,
        ProfileSettings                 = 215,
        GamepadInterface                = 216,
        FrontendMusic                   = 217,
        GenericStringManager            = 218,
        GlobalTextures                  = 219,
        AutoSaveHandler                 = 220,
        MUBootupFlow                    = 222,
        MULoadSaveProfileFlow           = 223,
        MUAutoSaveFlow                  = 227,

        /* Number of singleton objects */
        MAX_COUNT                       = 230
    };

    template <ESingletonType _Type, class _Class = void>
    class SingletonVar {
    public:
        static inline _Class * Get() {
            return reinterpret_cast<_Class *>(GetSingletonObject(_Type));
        }

        static inline void Set(_Class *value) {
            SetSingletonObject(_Type, reinterpret_cast<intptr_t>(value));
        }
    };

    using Framework = SingletonVar<ESingletonType::Framework, IFramework>;
    using UserCommandProxy = SingletonVar<ESingletonType::UserCommandProxy, IUserCommandProxy>;
}
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

    static bool GetGameInfo(RSDSEntry &ppGameInfo);

    static int Version();
};