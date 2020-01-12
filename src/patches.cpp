
#include "main.h"

//#define USE_GUI
//#define DRAW_GUI 0

#include "imgui.h"

#ifdef USE_GUI
#include "hook_gui.h"
#endif

#include <shellapi.h>

IDirect3D9* pD3D;
IDirect3DDevice9* pD3DDevice;

IDirect3D9Hook* pD3DHook;
IDirect3DDevice9Hook* pD3DDeviceHook;

CDriv3r* pDriv3r = 0;


IUserCommandProxy* pUserCmdProxy;
IUserCommandProxyHook* pUserCmdProxyHook;

WNDPROC hProcOld;
LRESULT APIENTRY WndProcNew(HWND, UINT, WPARAM, LPARAM);

static char gamedir[MAX_PATH]{ NULL };

static int gameversion = 0;

static fxModule gamemodule;
static fxModule mymodule;

#ifdef USE_GUI
static bool g_bGUIWaiting = false;

static bool g_bGUIReady = false;
static bool g_bGUIFocus = false;

static void DrawGUI() {
#if DRAW_GUI
    if (g_bGUIWaiting) {
        LogFile::WriteLine("Initializing ImGui...");
        gui::Initialize(hamster::GetMainWindow(), pD3DDevice);
        g_bGUIReady = true;
        g_bGUIWaiting = false;
    }

    if (g_bGUIReady) {
        gui::BeginFrame();
        gui::Update();
        gui::Render();
        gui::EndFrame();
    }
#endif
}
#endif

static void InitGameModule(HINSTANCE hInstance) {
    if (!fxModule::Initialize(gamemodule, hInstance))
        LogFile::WriteLine("Couldn't initialize the game's module info!");
}

struct _D3_SHADER_INFO {
    UINT32 offset;
    UINT32 size;
};

struct _D3_SHADER {
    UINT32 index;
    INT32 type;
    _D3_SHADER_INFO info[D3_NUM_SUPPORTED_VERSIONS];
};

const _D3_SHADER vs_shaders[] = {
    { 1, -1,{ { 0x6FD200, 0x0138 },{ 0x71C828, 0x0138 } } },
    { 5, -1,{ { 0x6FD338, 0x00C0 },{ 0x71C960, 0x006C } } },
    { 8, -1,{ { 0x6FD3A8, 0x0050 },{ 0x71C9D0, 0x0050 } } },
    { 3, -1,{ { 0x6FD3F8, 0x01D4 },{ 0x71CA20, 0x01D4 } } },
    { 4, -1,{ { 0x6FD5D0, 0x028C },{ 0x71CBF8, 0x028C } } },
    { 9, -1,{ { 0x6FD860, 0x0154 },{ 0x71CE88, 0x0154 } } },
};

const _D3_SHADER ps_shaders[] = {
    { 2, -1,{ { 0x6FD9B8, 0x0058 },{ 0x71CFE0, 0x0058 } } },
};

const _D3_SHADER shader_programs[] = {
    { 0,  0,{ { 0x6FDA10, 0x0830 },{ 0x71D038, 0x0830 } } },

    { 1,  1,{ { 0x6FE240, 0x20DC },{ 0x71D868, 0x20DC } } },
    { 2,  1,{ { 0x700320, 0x244C },{ 0x71F948, 0x244C } } },
    { 3,  1,{ { 0x702770, 0x2464 },{ 0x721D98, 0x2464 } } },
    { 5,  1,{ { 0x704BD8, 0x287C },{ 0x724200, 0x287C } } },
    { 6,  1,{ { 0x707458, 0x28A8 },{ 0x726A80, 0x28A8 } } },

    { 9,  2,{ { 0x709D00, 0x3170 },{ 0x729328, 0x3170 } } },
    { 7,  2,{ { 0x70CE70, 0x2A84 },{ 0x72C498, 0x2A84 } } },

    { 4,  1,{ { 0x70F8F8, 0x1920 },{ 0x72EF20, 0x1920 } } },

    { 16, 3,{ { 0x711218, 0x1900 },{ 0x730840, 0x1900 } } },

    { 17, 1,{ { 0x712B18, 0x1924 },{ 0x732140, 0x285C } } },
    { 10, 1,{ { 0x718D60, 0x1840 },{ 0x739038, 0x1840 } } },
    { 11, 1,{ { 0x71A5A0, 0x19C4 },{ 0x73A878, 0x19C4 } } },
    { 22, 1,{ { 0x71BF68, 0x1840 },{ 0x73C240, 0x1840 } } },
    { 12, 1,{ { 0x71D7A8, 0x1860 },{ 0x73DA80, 0x1860 } } },
    { 13, 1,{ { 0x71F008, 0x1A84 },{ 0x73F2E0, 0x19D4 } } },
    { 21, 1,{ { 0x720A90, 0x1840 },{ 0x740CB8, 0x1840 } } },
    { 23, 1,{ { 0x749EF8, 0x19E0 },{ 0x76D548, 0x2A84 } } },
    { 24, 1,{ { 0x74B8D8, 0x19E0 },{ 0x76FFD0, 0x2A84 } } },
    { 25, 1,{ { 0x7507D8, 0x19FC },{ 0x778188, 0x2AD8 } } },
    { 26, 1,{ { 0x7521D8, 0x19FC },{ 0x77AC60, 0x2AD8 } } },
    { 14, 1,{ { 0x73F528, 0x1A90 },{ 0x762C30, 0x19D4 } } },
    { 8,  1,{ { 0x740FB8, 0x18D0 },{ 0x764608, 0x18D0 } } },

    { 43, 4,{ { 0x746A58, 0x16DC },{ 0x76A0A8, 0x16DC } } },

    { 30, 5,{ { 0x7222D0, 0x26AC },{ 0x7424F8, 0x26AC } } },
    { 31, 5,{ { 0x724980, 0x2D1C },{ 0x744BA8, 0x3A28 } } },
    { 32, 5,{ { 0x7276A0, 0x2CF4 },{ 0x7485D0, 0x3A20 } } },
    { 33, 5,{ { 0x72A398, 0x1F28 },{ 0x74BFF0, 0x2C34 } } },
    { 29, 5,{ { 0x72C2C0, 0x1FB0 },{ 0x74EC28, 0x2D44 } } },

    { 34, 6,{ { 0x72E270, 0x2200 },{ 0x751970, 0x2200 } } },
    { 35, 6,{ { 0x730470, 0x36E4 },{ 0x753B70, 0x36F0 } } },
    { 36, 6,{ { 0x733B58, 0x3860 },{ 0x757260, 0x3860 } } },
    { 37, 6,{ { 0x7373B8, 0x388C },{ 0x75AAC0, 0x388C } } },
    { 38, 6,{ { 0x73AC48, 0x2200 },{ 0x75E350, 0x2200 } } },
    { 39, 6,{ { 0x742888, 0x227C },{ 0x765ED8, 0x227C } } },

    { 40, 7,{ { 0x744B08, 0x1F4C },{ 0x768158, 0x1F4C } } },

    { 18, 8,{ { 0x714440, 0x18E4 },{ 0x7349A0, 0x18E4 } } },
    { 19, 8,{ { 0x715D28, 0x1814 },{ 0x736288, 0x16D4 } } },
    { 20, 8,{ { 0x717540, 0x1820 },{ 0x737960, 0x16D4 } } },
    { 27, 8,{ { 0x74D2B8, 0x1A90 },{ 0x772A58, 0x2B94 } } },
    { 28, 8,{ { 0x74ED48, 0x1A90 },{ 0x7755F0, 0x2B94 } } },

    { 44, 9,{ { 0x73E748, 0x0DDC },{ 0x761E50, 0x0DDC } } },
    { 45, 9,{ { 0x748138, 0x0C00 },{ 0x76B788, 0x0C00 } } },

    { 46, 1,{ { 0x73CE48, 0x18FC },{ 0x760550, 0x18FC } } },

    { 47, 10,{ { 0x748D38, 0x11BC },{ 0x76C388, 0x11BC } } },
    { 41, 10,{ { 0x753BD8, 0x1368 },{ 0x77D738, 0x1368 } } },
    { 42, 10,{ { 0x754F40, 0x1378 },{ 0x77EAA0, 0x1378 } } },
};

LPFNDIRECTINPUT8CREATE lpDI8Create;

// Export as 'DirectInput8Create' so we can hook into Driv3r
#pragma comment(linker, "/EXPORT:DirectInput8Create=_DirectInput8Create_Impl@20")
HRESULT WINAPI DirectInput8Create_Impl(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter) {
    /*
        Not much to hook here for the moment
    */
    return lpDI8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}

bool SubclassGameWindow(HWND gameWnd, WNDPROC pWndProcNew, WNDPROC* ppWndProcOld)
{
    if (gameWnd != NULL)
    {
        WNDPROC hProcOld = (WNDPROC)GetWindowLong(gameWnd, GWL_WNDPROC);

        *ppWndProcOld = hProcOld;

        if (hProcOld != NULL && SetWindowLong(gameWnd, GWL_WNDPROC, (LONG)pWndProcNew))
            return true;
    }
    return false;
};

void HandlePanicButton(DWORD vKey)
{
    static int exit_presses = 0;

    if (vKey == VK_OEM_3)
    {
        if (++exit_presses == 2)
        {
            // ABORT MISSION!!!
            ExitProcess(EXIT_SUCCESS);
        }
    }
    else {
        exit_presses = 0;
    }
}

static DWORD g_LastKey = 0;

bool HandleKeyPress(DWORD vKey)
{
    g_LastKey = vKey;

    HandlePanicButton(vKey);

    switch (vKey) {
#ifdef USE_GUI
    case VK_F3:
    {
        g_bGUIFocus = !g_bGUIFocus;
        return true;
    }
#endif
    case VK_F5:
    {
        LOG("Testing the file chunker hook...");
        LOG(" - Creating new chunker...");
        CFileChunker fileChunker = CFileChunker();

        LogFile::Format(" - Chunk count: %d\n", fileChunker.GetChunkCount());

        LOG(" - Testing complete!");
        return true;
    }
    case VK_F6:
    {
        LOG("Dumping shaders...");

        char shadersDir[MAX_PATH] = { NULL };
        char* appPath = hook::VTHook(hamster::GetSingletonObject(hamster::ESingletonType::SystemConfig))[16].Call<char*>();

        char* shadersVer = (gameversion == __DRIV3R_V100) ? "v100" : "v120";

        sprintf(shadersDir, "%s\\shaders\\%s", appPath, shadersVer);

        SHCreateDirectoryExA(NULL, shadersDir, NULL);

        struct _D3_SHADER_EXPORT {
            const char* fmtStr;
            const _D3_SHADER* shaders;
            int nShaders;
        };

        const _D3_SHADER_EXPORT outShaders[] = {
            { "%s\\vs\\", vs_shaders, _countof(vs_shaders) },
            { "%s\\ps\\", ps_shaders, _countof(ps_shaders) },
            { "%s\\", shader_programs, _countof(shader_programs) },
        };

        //--char chunkerPath[MAX_PATH] = { NULL };
        //--sprintf(chunkerPath, "%s\\AllShaders.chunk", &shadersDir);
        //--
        //--LOG(" - Initializing shader chunker...");
        //--CFileChunker shaderChunker;
        //--int shaderIndex = 0;
        //--
        //--for (int i = 0; i < sizeof(CFileChunker); i += 4)
        //--    *(DWORD*)((BYTE*)&shaderChunker + i) = 0xDEC015A1;
        //--
        //--shaderChunker = CFileChunker();

        for (auto out_ : outShaders)
        {
            char outDir[MAX_PATH];
            sprintf(outDir, out_.fmtStr, &shadersDir);

            SHCreateDirectoryExA(NULL, outDir, NULL);

            for (int i = 0; i < out_.nShaders; i++)
            {
                auto shader = out_.shaders[i];

                auto offset = shader.info[gameversion].offset;
                auto size = shader.info[gameversion].size;

                // better safe then sorry
                if (offset == 0 || size == 0)
                    continue;
                if (shader.type != -1 && *(DWORD*)offset != 0xFEFF0901)
                {
                    // that's what ya get for hardcoding offsets, dummy!
                    LogFile::Format(" - Bad shader definition @ 0x%08X (size: 0x%08X), cannot dump shader!\n", offset, size);
                    continue;
                }

                //--LOG(" - Adding shader chunk...");
                //--shaderChunker.AddChunk((LPVOID)shader.info[gameVersion].offset, shader.info[gameVersion].size, CHUNKCOPYTYPE_COPY, 1024);
                //--shaderChunker.SetChunkVersion(shaderIndex, (BYTE)shader.type);
                //--
                //--LogFile::Format(" - Size: %08X\n", shaderChunker.GetPaddedChunkSize(shaderIndex));
                //--
                //--shaderIndex++;

                bool disassemblyTesting = true;

                if (disassemblyTesting)
                {
                    char outPath[MAX_PATH] = { NULL };
                    sprintf(outPath, (shader.type != -1) ? "%s\\%.4d_%.2d.fx" : "%s\\%.4d.hlsl", outDir, shader.index, shader.type);

                    LPD3DXEFFECT pEffect;
                    LPD3DXBUFFER outBuffer;

                    FILE* outFile = fopen(outPath, "wb");

                    if (shader.type != -1)
                    {
                        if (D3DXCreateEffect(hamster::GetD3DDevice(), (LPCVOID)offset, size, NULL, NULL, D3DXSHADER_NO_PRESHADER, NULL, &pEffect, &outBuffer) == D3D_OK)
                        {
                            if (D3DXDisassembleEffect(pEffect, FALSE, &outBuffer) == D3D_OK)
                            {
                                LPVOID lpBuffer = outBuffer->GetBufferPointer();
                                DWORD outSize = outBuffer->GetBufferSize() - 1; // don't include the null terminator

                                if (fwrite(lpBuffer, sizeof(BYTE), outSize, outFile) == outSize)
                                {
                                    LogFile::Format(" - Successfully disassembled effect file \"%s\"!\n", outPath);
                                }
                                else
                                {
                                    LogFile::Format(" - Failed to write disassembled effect file \"%s\"!\n", outPath);
                                }
                            }
                            else
                            {
                                LogFile::Format(" - Failed to disassemble effect file \"%s\"!\n", outPath);
                            }

                            // release our effect
                            pEffect->Release();
                        }
                        else
                        {
                            LogFile::Format(" - Failed to compile effect file \"%s\"!\n", outPath);
                        }
                    }
                    else
                    {
                        if (D3DXDisassembleShader((LPDWORD)offset, FALSE, NULL, &outBuffer) == D3D_OK)
                        {
                            LPVOID lpBuffer = outBuffer->GetBufferPointer();
                            DWORD outSize = outBuffer->GetBufferSize() - 1; // don't include the null terminator

                            if (fwrite(lpBuffer, sizeof(BYTE), outSize, outFile) == outSize)
                            {
                                LogFile::Format(" - Successfully disassembled shader file \"%s\"!\n", outPath);
                            }
                            else
                            {
                                LogFile::Format(" - Failed to write disassembled shader file \"%s\"!\n", outPath);
                            }
                        }
                        else
                        {
                            LogFile::Format(" - Failed to disassemble shader file \"%s\"!\n", outPath);
                        }
                    }

                    fclose(outFile);
                }
                else
                {
                    char outPath[MAX_PATH] = { NULL };
                    sprintf(outPath, (shader.type != -1) ? "%s\\%.4d_%.2d.cso" : "%s\\%.4d.cso", outDir, i, shader.type);

                    FILE* outFile = fopen(outPath, "wb");

                    // write index
                    fwrite((LPVOID)&shader.index, sizeof(BYTE), 4, outFile);

                    if (fwrite((LPVOID)offset, sizeof(BYTE), size, outFile) == size)
                    {
                        LogFile::Format(" - Successfully dumped shader file \"%s\"!\n", outPath);
                    }
                    else
                    {
                        LogFile::Format(" - Failed to dump shader file \"%s\"!\n", outPath);
                    }

                    fclose(outFile);
                }
            }
        }

        //--for (int i = 0; i < shaderIndex; i++)
        //--    shaderChunker.SetChunkContext(i, 0x52444853);
        //--
        //--FILE *outFile = fopen(chunkerPath, "wb");
        //--
        //--int outSize = sizeof(CFileChunker);
        //--
        //--LOG(" - Writing shader chunker to file...");
        //--if (fwrite(&shaderChunker, sizeof(BYTE), outSize, outFile) == outSize)
        //--{
        //--    LogFile::Format(" - Successfully dumped shader chunker \"%s\"!\n", chunkerPath);
        //--}
        //--else
        //--{
        //--    LogFile::Format(" - Failed to dump shader chunker \"%s\"!\n", chunkerPath);
        //--}
        //--
        //--fclose(outFile);
        return true;
    }
    case VK_F8:
    {
        auto addr = (intptr_t**)hamster::GetSingletonObjectsPointer();

        for (int i = 0; i < static_cast<int>(hamster::ESingletonType::MAX_COUNT); i++) {
            auto ptr = &addr[i];

            if (*ptr == nullptr)
                continue;

            LogFile::Format("singleton[%02X] @ %08X : %08X -> %08X\n", i, ptr, *ptr, **ptr /* vtable */);
        }

        return true;
    }
    case 'v':
    case 'V':
    {
        D3DVIEWPORT9 pViewport;

        if (pD3DDevice->GetViewport(&pViewport) == D3D_OK)
        {
            pViewport.MinZ = 0.001f;

            if (pD3DDevice->SetViewport(&pViewport) == D3D_OK)
            {
                pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 0.0f, 0);
                pD3DDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 0.0f, 0);
            }
        }
    } break; // no need to handle this completely
    }

    return false;
}

FnHook<int, DWORD, DWORD, DWORD, LPD3DXEFFECTPOOL>  lpfnInitShaderThing1;
FnHook<int, DWORD, DWORD, DWORD, LPD3DXEFFECTPOOL>  lpfnInitShaderThing2;
FnHook<int, DWORD, DWORD, DWORD, LPD3DXEFFECTPOOL>  lpfnInitShaderThing3;
FnHook<int, DWORD, DWORD, DWORD, LPD3DXEFFECTPOOL>  lpfnInitShaderThing4;
FnHook<int, DWORD, DWORD, DWORD, LPD3DXEFFECTPOOL>  lpfnInitShaderThing5;
FnHook<int, DWORD, DWORD, DWORD, LPD3DXEFFECTPOOL>  lpfnInitShaderThing6;
FnHook<int, DWORD, DWORD, DWORD, LPD3DXEFFECTPOOL>  lpfnInitShaderThing7;
FnHook<int, DWORD, DWORD, DWORD, LPD3DXEFFECTPOOL>  lpfnInitShaderThing8;
FnHook<int, DWORD, DWORD, DWORD, LPD3DXEFFECTPOOL>  lpfnInitShaderThing9;
FnHook<int, DWORD, DWORD, DWORD, LPD3DXEFFECTPOOL>  lpfnInitShaderThing10;
FnHook<int, DWORD, DWORD>                         lpfnInitGlobalShaderThing;

DWORD gpuShaders[47];

struct SShaderFX {
    LPD3DXEFFECT pEffect;
    LPD3DXBUFFER pErrors;
    LPD3DXEFFECTPOOL pPool;
    D3DXHANDLE pTechnique;
};

struct SGlobalModelEffect {
    int unk_00;
    int unk_04; // uid?
    SShaderFX* shaderFX;
    LPD3DXEFFECTPOOL pEffectPool;
    int unk_10;
    int unk_14;
    int unk_18;
    int unk_1C;
};

struct SModelEffect {
    SShaderFX* pShader;
    UINT uid;
    UINT unk_08;
    void* pTexture;
};

class effectManager {
protected:
    SGlobalModelEffect* pGlobalEffects[10];
    size_t nGlobalEffects;

    SModelEffect* pModelEffects[64];
    size_t nModelEffects;
};

class effectManagerHandler : public effectManager {
protected:
    int InitShader(DWORD type, DWORD index, DWORD offset, DWORD size)
    {
        int result = 0;

        if (type == 0)
        {
            result = lpfnInitGlobalShaderThing(this, offset, size);
            LogFile::Format(" - Initialized global shader (offset: 0x%08X, size: 0x%08X), Result = 0x%08X\n",
                offset, size, result);
        }
        else
        {
            auto unk = pGlobalEffects[0]->pEffectPool;

            switch (type)
            {
            case 1:
                result = lpfnInitShaderThing1(this, offset, size, index, unk);
                break;
            case 2:
                result = lpfnInitShaderThing2(this, offset, size, index, unk);
                break;
            case 3:
                result = lpfnInitShaderThing3(this, offset, size, index, unk);
                break;
            case 4:
                result = lpfnInitShaderThing4(this, offset, size, index, unk);
                break;
            case 5:
                result = lpfnInitShaderThing5(this, offset, size, index, unk);
                break;
            case 6:
                result = lpfnInitShaderThing6(this, offset, size, index, unk);
                break;
            case 7:
                result = lpfnInitShaderThing7(this, offset, size, index, unk);
                break;
            case 8:
                result = lpfnInitShaderThing8(this, offset, size, index, unk);
                break;
            case 9:
                result = lpfnInitShaderThing9(this, offset, size, index, unk);
                break;
            case 10:
                result = lpfnInitShaderThing10(this, offset, size, index, unk);
                break;
            }
            LogFile::Format(" - Initialized shader (type: %d, offset: 0x%08X, size: 0x%08X, index: %d, unk: 0x%08X), Result = 0x%08X\n",
                type, offset, size, index, unk, result);
        }
        return result;
    }

    int InitShaderFX()
    {
        if (gameversion == __DRIV3R_V100)
        {
            /* Loads shaders from AllShaders.chunk */
            CFileChunker shadersChunk = CFileChunker();

            int errorCode = 0;

            LogFile::WriteLine("Loading shaders chunk...");
            if (shadersChunk.OpenChunks("shaders\\AllShaders.chunk"))
            {
                int nShaders = shadersChunk.GetChunkCount();

                LogFile::Format(" - Count: %d\n", nShaders);
                for (int i = 0; i < nShaders; i++)
                {
                    if (shadersChunk.GetChunkContext(i) == 0x53555047)
                    {
                        int type = shadersChunk.GetChunkVersion(i);

                        DWORD size = shadersChunk.GetChunkSize(i) - 4;
                        DWORD bufSize = shadersChunk.GetPaddedChunkSize(i);

                        BYTE* buffer = new BYTE[bufSize];
                        shadersChunk.GetChunkData(i, buffer, CHUNKCOPYTYPE_LOAD);

                        while (shadersChunk.IsBusy());

                        int index = *(DWORD*)buffer;
                        DWORD offset = (DWORD)(buffer + 4);

                        gpuShaders[i] = offset;

                        LogFile::Format(" - Initializing shader %d %d %X %X\n", type, index, offset, size);
                        int result = this->InitShader(type, index, offset, size);

                        if (result < 0) {
                            // abort mission!
                            errorCode = result;
                            break;
                        }
                    }
                }

                shadersChunk.Release();
            }
            else
            {
                MessageBoxW(hamster::GetMainWindow(), L"FATAL ERROR: Cannot open the shaders file!", L"D3Hook", MB_OK | MB_ICONERROR);
                return E_FAIL;
            }

            // something went wrong, abort
            if (errorCode != 0)
                return errorCode;
        }
        else
        {
            for (auto shader : shader_programs) {
                /* Loads shader from EXE */
                auto shaderInfo = shader.info[gameversion];

                int result = this->InitShader(shader.type, shader.index, shaderInfo.offset, shaderInfo.size);

                // don't continue if this fails
                if (result < 0)
                    return result;
            }
        }

        // what the actual fuck is this structure though
        fiero::Type<int[2880]> bins(addressof(0x8E6718, 0x8ACCC8));

        memset(bins, 0, bins.length());

        bins[75] = 1;
        bins[80] = 2;
        bins[95] = 3;
        bins[81] = 5;
        bins[96] = 6;
        bins[99] = 9;

        bins[1413] = 24;
        bins[1653] = 28;

        bins[620] = 11;

        bins[735] = 12;
        bins[740] = 12;
        bins[755] = 12;

        bins[1533] = 26;

        bins[1815] = 30;
        bins[1840] = 32;
        bins[1825] = 31;

        bins[2055] = 34;
        bins[2056] = 35;
        bins[2061] = 36;
        bins[2076] = 37;

        LogFile::WriteLine("Finished initializing shaders!");
        return 0;
    }
public:
    static bool Install() {
        //
        // look how easy this is to do with the vanilla version!
        //
        lpfnInitShaderThing1 = addressof(0x5DD620, 0x5D3D30);
        lpfnInitShaderThing2 = addressof(0x5DD6B0, 0x5D3DA0);
        lpfnInitShaderThing3 = addressof(0x5DD740, 0x5D3E30);
        lpfnInitShaderThing4 = addressof(0x5DD7D0, 0x5D3EA0);
        lpfnInitShaderThing5 = addressof(0x5DD860, 0x5D3F10);
        lpfnInitShaderThing6 = addressof(0x5DD8F0, 0x5D3F80);
        lpfnInitShaderThing7 = addressof(0x5DD980, 0x5D3FF0);
        lpfnInitShaderThing8 = addressof(0x5DDA10, 0x5D4070);
        lpfnInitShaderThing9 = addressof(0x5DDAA0, 0x5D40E0);
        lpfnInitShaderThing10 = addressof(0x5DDB30, 0x5D4150);
        lpfnInitGlobalShaderThing = addressof(0x5DE730, 0x5D30B0);

        if (gameversion == __DRIV3R_V120) {
            //
            // ...but for patch 2, we have to baptise the fucking spaghetti monster!
            //
            InstallPatch("Spaghetti monster patch [1/12]", {
                0x8B, 0xCE,                     // mov ecx, esi
                0x90, 0x90, 0x90, 0x90, 0x90,   // nop(5)           ; space for our call injection
                0x85, 0xC0,                     // test eax, eax
                }, { 0x5D2600 });

            //
            // converting these to __thiscall is sooo much fun -.-
            //
            InstallPatch("Spaghetti monster patch [2/12]", {
                // we're so incredibly tight on space, holy shit
                0x58,                           // 00:-004 pop eax          ; remove return address (note our stack pointer deficiency)
                0x51,                           // 01:-008 push ecx         ; make the stack of spaghetti happy
                0x50,                           // 02:-004 push eax         ; restore return address

                // recreate the original entry point in as few bytes as possible
                0x53,                           // 03: 000 push ebx
                0x55,                           // 04: 004 push ebp
                0x51,                           // 05: 008 push ecx         ; get ECX into EBP
                0x5D,                           // 06: 00C pop ebp          ; voila!

                // unfortunately we added a byte, so shift everything down as tightly-packed as possible
                0x56,                           // 07: 008 push esi
                0x57,                           // 08: 00C push edi
                0x6A, 0x40,                     // 09: 010 push 40h
                0xE8, 0xF5, 0xF3, 0x00, 0x00,   // 11: 014 call 5E24B5      ; remove 1 byte from the RVA
                0x5F,                           // 16: 014 pop edi          ; clean up the stack (this saves us 2 bytes)

                // voila! the stack is now cleaned up and re-aligned :)
                0x33, 0xFF,                     // 17: 010 xor edi, edi     ; almost there!
                0x90,                           // 19: 010 nop              ; phew! just a single byte to spare ;)
                }, { 0x5D30B0 });

            //
            // well, here we fucking go...sing it, programmer man!
            //

            // 10 fucked up compiler calls; take one down, patch it around, 9 fucked up compiler calls
            InstallPatch("Spaghetti monster patch [3/12]", {
                0x8B, 0xF9,                     // mov edi, ecx
                0xE8, 0x77, 0xE7, 0x00, 0x00,   // call 5E24B5
                0x5B,                           // pop ebx
                0x33, 0xDB,                     // xor ebx, ebx
                }, { 0x5D3D37 });
            // 9 fucked up compiler calls; take one down, patch it around, 8 fucked up compiler calls
            InstallPatch("Spaghetti monster patch [4/12]", {
                0x8B, 0xF9,                     // mov edi, ecx
                0xE8, 0x07, 0xE7, 0x00, 0x00,   // call 5E24B5
                0x5B,                           // pop ebx
                0x33, 0xDB,                     // xor ebx, ebx
                }, { 0x5D3DA7 });
            // 8 fucked up compiler calls; take one down, patch it around, 7 fucked up compiler calls
            InstallPatch("Spaghetti monster patch [5/12]", {
                0x8B, 0xF9,                     // mov edi, ecx
                0xE8, 0x77, 0xE6, 0x00, 0x00,   // call 5E24B5
                0x5B,                           // pop ebx
                0x33, 0xDB,                     // xor ebx, ebx
                }, { 0x5D3E37 });
            // 7 fucked up compiler calls; take one down, patch it around, 6 fucked up compiler calls
            InstallPatch("Spaghetti monster patch [6/12]", {
                0x8B, 0xF9,                     // mov edi, ecx
                0xE8, 0x07, 0xE6, 0x00, 0x00,   // call 5E24B5
                0x5B,                           // pop ebx
                0x33, 0xDB,                     // xor ebx, ebx
                }, { 0x5D3EA7 });
            // 6 fucked up compiler calls; take one down, patch it around, 5 fucked up compiler calls
            InstallPatch("Spaghetti monster patch [7/12]", {
                0x8B, 0xF9,                     // mov edi, ecx
                0xE8, 0x97, 0xE5, 0x00, 0x00,   // call 5E24B5
                0x5B,                           // pop ebx
                0x33, 0xDB,                     // xor ebx, ebx
                }, { 0x5D3F17 });
            // 5 fucked up compiler calls; take one down, patch it around, 4 fucked up compiler calls
            InstallPatch("Spaghetti monster patch [8/12]", {
                0x8B, 0xF9,                     // mov edi, ecx
                0xE8, 0x27, 0xE5, 0x00, 0x00,   // call 5E24B5
                0x5B,                           // pop ebx
                0x33, 0xDB,                     // xor ebx, ebx
                }, { 0x5D3F87 });
            // 4 fucked up compiler calls; take one down, patch it around, 3 fucked up compiler calls
            InstallPatch("Spaghetti monster patch [9/12]", {
                0x8B, 0xF9,                     // mov edi, ecx
                0xE8, 0xB7, 0xE4, 0x00, 0x00,   // call 5E24B5
                0x5B,                           // pop ebx
                0x33, 0xDB,                     // xor ebx, ebx
                }, { 0x5D3FF7 });
            // 3 fucked up compiler calls; take one down, patch it around, 2 fucked up compiler calls
            InstallPatch("Spaghetti monster patch [10/12]", {
                0x8B, 0xF9,                     // mov edi, ecx
                0xE8, 0x37, 0xE4, 0x00, 0x00,   // call 5E24B5
                0x5B,                           // pop ebx
                0x33, 0xDB,                     // xor ebx, ebx
                }, { 0x5D4077 });
            // 2 fucked up compiler calls; take one down, patch it around, 1 fucked up compiler call
            InstallPatch("Spaghetti monster patch [11/12]", {
                0x8B, 0xD9,                     // mov ebx, ecx
                0xE8, 0xCA, 0xE3, 0x00, 0x00,   // call 5E24B5
                0x5F,                           // pop edi
                0x33, 0xFF,                     // xor edi, edi
                }, { 0x5D40E4 });
            // 1 fucked up compiler call; take one down, patch it around, NO MORE FUCKED UP COMPILER CALLS!!!
            InstallPatch("Spaghetti monster patch [12/12]", {
                0x8B, 0xD9,                     // mov ebx, ecx
                0xE8, 0x5B, 0xE3, 0x00, 0x00,   // call 5E24B5
                0x5F,                           // pop edi
                }, { 0x5D4153 });
            //
            // I will NEVER do this again.
            //
        }

        //
        // This is the only fucking thing I will ever support for Patch 2, I swear!
        //
        InstallCallback("effectManager::InitShaderFX", "Allows for custom shader processing.",
            &InitShaderFX, {
                addressof<CALL>(0x5DEFA9, 0x5D2602),
            }
        );

        return true;
    }
};

FnHook<void, MeshGroup*, void*> lpfnInitLodInstance;

/*
    NOT COMPATIBLE WITH PATCH 2
*/
class DrawListHandler {
public:
    static void initLodInstances(void* model, void* inst, ModelContainer* container) {
        // BUGFIX: "Taxi taillight bug"
        int lod = 0;

        switch (*(WORD*)((BYTE*)inst + 0x62) & 0x1F) {
        case 1:
            lod = 4;
            break;
        case 2:
            lod = 0;
            break;
        case 4:
            lod = 1;
            break;
        case 8:
            lod = 2;
            break;
        default:
            return;
        }

        LodEntry& entry = container->Entries[lod];

        auto pModels = entry.MeshGroups;
        auto nModels = entry.nMeshGroups;

        if (pModels != nullptr)
        {
            for (int i = 0; i < nModels; i++)
                lpfnInitLodInstance(&pModels[i], model);
        }
    }

    static bool Install() {
        switch (gameversion)
        {
        case __DRIV3R_V100:
        {
            lpfnInitLodInstance = 0x5EB0A0;

            InstallPatch({
                0x50,                               // 00: push eax
                0x53,                               // 01: push ebx
                0x56,                               // 02: push esi
                0x90, 0x90, 0x90, 0x90, 0x90,       // 03: nop(5)       ; space for our call injection
                0x83, 0xC4, 0x0C,                   // 08: add esp, 0Ch
                0x90, 0x90,                         // 11: nop(2)
                0xE9,                               // 13: jmp [...]    ; ja -> jmp
                }, { 0x5EB19A });

            InstallCallback("initLodInstances", "Fixes the taxi taillight bug",
                &initLodInstances, {
                    cbHook<CALL>(0x5EB19A + 3),     // see patch above
                }
            );
        } return true;
        default:
            HANDLER_CANNOT_BE_IMPLEMENTED_BECAUSE_PATCH_2_SUCKS();
            return false;
        }
    }
};

struct StateParamsHolder {
    int type;
    int value;

    inline StateParamsHolder(int type, int value)
        : type(type), value(value) {}
    inline StateParamsHolder(int type, float value)
        : type(type), value(*reinterpret_cast<int*>(&value)) {}
};

class renderer;
class rendererHandler;

class d3dStateManager;

static char* D3DVAL2STR(int etype, int type, int value) {
    switch (etype) {
        // D3DRS
    case 0:
    {
        switch (type) {
        case D3DRS_ZENABLE:
        case D3DRS_ZWRITEENABLE:
        case D3DRS_ALPHATESTENABLE:
        case D3DRS_DITHERENABLE:
        case D3DRS_ALPHABLENDENABLE:
        case D3DRS_FOGENABLE:
        case D3DRS_SPECULARENABLE:
        case D3DRS_RANGEFOGENABLE:
        case D3DRS_STENCILENABLE:
        case D3DRS_NORMALIZENORMALS:
        case D3DRS_CLIPPLANEENABLE:
        case D3DRS_POINTSPRITEENABLE:
        case D3DRS_POINTSCALEENABLE:
        case D3DRS_MULTISAMPLEANTIALIAS:
        case D3DRS_INDEXEDVERTEXBLENDENABLE:
        case D3DRS_SCISSORTESTENABLE:
        case D3DRS_ANTIALIASEDLINEENABLE:
        case D3DRS_ENABLEADAPTIVETESSELLATION:
        case D3DRS_TWOSIDEDSTENCILMODE:
        case D3DRS_SEPARATEALPHABLENDENABLE:
            return (value) ? "TRUE" : "FALSE";
        }
    } break;
    }

    static char _vbuf[32]{ 0 };

    sprintf(_vbuf, "0x%02X", value);
    return _vbuf;
}
static char* D3DSAMP_STRING(int type) {
    switch (type) {
        ENUMCASE2STR(D3DSAMP_ADDRESSU)
            ENUMCASE2STR(D3DSAMP_ADDRESSV)
            ENUMCASE2STR(D3DSAMP_ADDRESSW)
            ENUMCASE2STR(D3DSAMP_BORDERCOLOR)
            ENUMCASE2STR(D3DSAMP_MAGFILTER)
            ENUMCASE2STR(D3DSAMP_MINFILTER)
            ENUMCASE2STR(D3DSAMP_MIPFILTER)
            ENUMCASE2STR(D3DSAMP_MIPMAPLODBIAS)
            ENUMCASE2STR(D3DSAMP_MAXMIPLEVEL)
            ENUMCASE2STR(D3DSAMP_MAXANISOTROPY)
            ENUMCASE2STR(D3DSAMP_SRGBTEXTURE)
            ENUMCASE2STR(D3DSAMP_ELEMENTINDEX)
            ENUMCASE2STR(D3DSAMP_DMAPOFFSET)
    }
    return "<???>";
}
static char* D3DTSS_STRING(int type) {
    switch (type) {
        ENUMCASE2STR(D3DTSS_COLOROP)
            ENUMCASE2STR(D3DTSS_COLORARG1)
            ENUMCASE2STR(D3DTSS_COLORARG2)
            ENUMCASE2STR(D3DTSS_ALPHAOP)
            ENUMCASE2STR(D3DTSS_ALPHAARG1)
            ENUMCASE2STR(D3DTSS_ALPHAARG2)
            ENUMCASE2STR(D3DTSS_BUMPENVMAT00)
            ENUMCASE2STR(D3DTSS_BUMPENVMAT01)
            ENUMCASE2STR(D3DTSS_BUMPENVMAT10)
            ENUMCASE2STR(D3DTSS_BUMPENVMAT11)
            ENUMCASE2STR(D3DTSS_TEXCOORDINDEX)
            ENUMCASE2STR(D3DTSS_BUMPENVLSCALE)
            ENUMCASE2STR(D3DTSS_BUMPENVLOFFSET)
            ENUMCASE2STR(D3DTSS_TEXTURETRANSFORMFLAGS)
            ENUMCASE2STR(D3DTSS_COLORARG0)
            ENUMCASE2STR(D3DTSS_ALPHAARG0)
            ENUMCASE2STR(D3DTSS_RESULTARG)
            ENUMCASE2STR(D3DTSS_CONSTANT)
    }
    return "<???>";
}
static char* D3DRS_STRING(int type) {
    switch (type) {
        ENUMCASE2STR(D3DRS_ZENABLE)
            ENUMCASE2STR(D3DRS_FILLMODE)
            ENUMCASE2STR(D3DRS_SHADEMODE)
            ENUMCASE2STR(D3DRS_ZWRITEENABLE)
            ENUMCASE2STR(D3DRS_ALPHATESTENABLE)
            ENUMCASE2STR(D3DRS_LASTPIXEL)
            ENUMCASE2STR(D3DRS_SRCBLEND)
            ENUMCASE2STR(D3DRS_DESTBLEND)
            ENUMCASE2STR(D3DRS_CULLMODE)
            ENUMCASE2STR(D3DRS_ZFUNC)
            ENUMCASE2STR(D3DRS_ALPHAREF)
            ENUMCASE2STR(D3DRS_ALPHAFUNC)
            ENUMCASE2STR(D3DRS_DITHERENABLE)
            ENUMCASE2STR(D3DRS_ALPHABLENDENABLE)
            ENUMCASE2STR(D3DRS_FOGENABLE)
            ENUMCASE2STR(D3DRS_SPECULARENABLE)
            ENUMCASE2STR(D3DRS_FOGCOLOR)
            ENUMCASE2STR(D3DRS_FOGTABLEMODE)
            ENUMCASE2STR(D3DRS_FOGSTART)
            ENUMCASE2STR(D3DRS_FOGEND)
            ENUMCASE2STR(D3DRS_FOGDENSITY)
            ENUMCASE2STR(D3DRS_RANGEFOGENABLE)
            ENUMCASE2STR(D3DRS_STENCILENABLE)
            ENUMCASE2STR(D3DRS_STENCILFAIL)
            ENUMCASE2STR(D3DRS_STENCILZFAIL)
            ENUMCASE2STR(D3DRS_STENCILPASS)
            ENUMCASE2STR(D3DRS_STENCILFUNC)
            ENUMCASE2STR(D3DRS_STENCILREF)
            ENUMCASE2STR(D3DRS_STENCILMASK)
            ENUMCASE2STR(D3DRS_STENCILWRITEMASK)
            ENUMCASE2STR(D3DRS_TEXTUREFACTOR)
            ENUMCASE2STR(D3DRS_WRAP0)
            ENUMCASE2STR(D3DRS_WRAP1)
            ENUMCASE2STR(D3DRS_WRAP2)
            ENUMCASE2STR(D3DRS_WRAP3)
            ENUMCASE2STR(D3DRS_WRAP4)
            ENUMCASE2STR(D3DRS_WRAP5)
            ENUMCASE2STR(D3DRS_WRAP6)
            ENUMCASE2STR(D3DRS_WRAP7)
            ENUMCASE2STR(D3DRS_CLIPPING)
            ENUMCASE2STR(D3DRS_LIGHTING)
            ENUMCASE2STR(D3DRS_AMBIENT)
            ENUMCASE2STR(D3DRS_FOGVERTEXMODE)
            ENUMCASE2STR(D3DRS_COLORVERTEX)
            ENUMCASE2STR(D3DRS_LOCALVIEWER)
            ENUMCASE2STR(D3DRS_NORMALIZENORMALS)
            ENUMCASE2STR(D3DRS_DIFFUSEMATERIALSOURCE)
            ENUMCASE2STR(D3DRS_SPECULARMATERIALSOURCE)
            ENUMCASE2STR(D3DRS_AMBIENTMATERIALSOURCE)
            ENUMCASE2STR(D3DRS_EMISSIVEMATERIALSOURCE)
            ENUMCASE2STR(D3DRS_VERTEXBLEND)
            ENUMCASE2STR(D3DRS_CLIPPLANEENABLE)
            ENUMCASE2STR(D3DRS_POINTSIZE)
            ENUMCASE2STR(D3DRS_POINTSIZE_MIN)
            ENUMCASE2STR(D3DRS_POINTSPRITEENABLE)
            ENUMCASE2STR(D3DRS_POINTSCALEENABLE)
            ENUMCASE2STR(D3DRS_POINTSCALE_A)
            ENUMCASE2STR(D3DRS_POINTSCALE_B)
            ENUMCASE2STR(D3DRS_POINTSCALE_C)
            ENUMCASE2STR(D3DRS_MULTISAMPLEANTIALIAS)
            ENUMCASE2STR(D3DRS_MULTISAMPLEMASK)
            ENUMCASE2STR(D3DRS_PATCHEDGESTYLE)
            ENUMCASE2STR(D3DRS_DEBUGMONITORTOKEN)
            ENUMCASE2STR(D3DRS_POINTSIZE_MAX)
            ENUMCASE2STR(D3DRS_INDEXEDVERTEXBLENDENABLE)
            ENUMCASE2STR(D3DRS_COLORWRITEENABLE)
            ENUMCASE2STR(D3DRS_TWEENFACTOR)
            ENUMCASE2STR(D3DRS_BLENDOP)
            ENUMCASE2STR(D3DRS_POSITIONDEGREE)
            ENUMCASE2STR(D3DRS_NORMALDEGREE)
            ENUMCASE2STR(D3DRS_SCISSORTESTENABLE)
            ENUMCASE2STR(D3DRS_SLOPESCALEDEPTHBIAS)
            ENUMCASE2STR(D3DRS_ANTIALIASEDLINEENABLE)
            ENUMCASE2STR(D3DRS_MINTESSELLATIONLEVEL)
            ENUMCASE2STR(D3DRS_MAXTESSELLATIONLEVEL)
            ENUMCASE2STR(D3DRS_ADAPTIVETESS_X)
            ENUMCASE2STR(D3DRS_ADAPTIVETESS_Y)
            ENUMCASE2STR(D3DRS_ADAPTIVETESS_Z)
            ENUMCASE2STR(D3DRS_ADAPTIVETESS_W)
            ENUMCASE2STR(D3DRS_ENABLEADAPTIVETESSELLATION)
            ENUMCASE2STR(D3DRS_TWOSIDEDSTENCILMODE)
            ENUMCASE2STR(D3DRS_CCW_STENCILFAIL)
            ENUMCASE2STR(D3DRS_CCW_STENCILZFAIL)
            ENUMCASE2STR(D3DRS_CCW_STENCILPASS)
            ENUMCASE2STR(D3DRS_CCW_STENCILFUNC)
            ENUMCASE2STR(D3DRS_COLORWRITEENABLE1)
            ENUMCASE2STR(D3DRS_COLORWRITEENABLE2)
            ENUMCASE2STR(D3DRS_COLORWRITEENABLE3)
            ENUMCASE2STR(D3DRS_BLENDFACTOR)
            ENUMCASE2STR(D3DRS_SRGBWRITEENABLE)
            ENUMCASE2STR(D3DRS_DEPTHBIAS)
            ENUMCASE2STR(D3DRS_WRAP8)
            ENUMCASE2STR(D3DRS_WRAP9)
            ENUMCASE2STR(D3DRS_WRAP10)
            ENUMCASE2STR(D3DRS_WRAP11)
            ENUMCASE2STR(D3DRS_WRAP12)
            ENUMCASE2STR(D3DRS_WRAP13)
            ENUMCASE2STR(D3DRS_WRAP14)
            ENUMCASE2STR(D3DRS_WRAP15)
            ENUMCASE2STR(D3DRS_SEPARATEALPHABLENDENABLE)
            ENUMCASE2STR(D3DRS_SRCBLENDALPHA)
            ENUMCASE2STR(D3DRS_DESTBLENDALPHA)
            ENUMCASE2STR(D3DRS_BLENDOPALPHA)
    }
    return "<???>";
};

static char* VALS2STR(char* type, char* value) {
    static char _buf1[48]{ 0 };
    static char _buf2[64]{ 0 };

    sprintf(_buf1, "%s,", type);
    sprintf(_buf2, "%-40s%s", _buf1, value);

    return _buf2;
};

static auto d3dStateManager_SetRenderStateParams = fiero::as<d3dStateManager>::func<int, int, StateParamsHolder*, int>(0x5DF810);
static auto d3dStateManager_SetTextureStageStateParams = fiero::as<d3dStateManager>::func<int, int, StateParamsHolder*, int>(0x5DF7B0);
static auto d3dStateManager_SetSamplerStateParams = fiero::as<d3dStateManager>::func<int, int, StateParamsHolder*, int>(0x5DF740);
static auto d3dStateManager_setGlobalStateParams = fiero::as<d3dStateManager>::func<void,
    StateParamsHolder*, int,
    StateParamsHolder*, int,
    StateParamsHolder*, int>(0x5DFFF0);

/*
    NOT COMPATIBLE WITH PATCH 2
*/
class d3dStateManager {
public:
    struct stateholder {
        StateParamsHolder texturestates[80];
        int texturestatescount;

        StateParamsHolder renderstates[80];
        int renderstatescount;

        StateParamsHolder samplerstates[80];
        int samplerstatescount;

        // yikes
        inline void addSamplerState(StateParamsHolder samplerState) {
            samplerstates[samplerstatescount++] = samplerState;
        };
    };

    stateholder states[51];
    int nStates;

    stateholder globalstate;

    void dumpstateparams(int state,
        StateParamsHolder* pRenderStates, int nRenderStates,
        StateParamsHolder* pT1States, int nT1States,
        StateParamsHolder* pT2States, int nT2States)
    {
        auto statePtr = (state == -1) ? &globalstate : &states[state];

        LogFile::Format("/* %08X */\n", statePtr);
        if (state == -1) {
            LogFile::WriteLine("GLOBAL_STATE : {");
        }
        else {
            LogFile::Format("STATE[%d] : {\n", state);
        }

        if (nRenderStates > 0)
        {
            LogFile::WriteLine("  RENDERSTATES : {");
            for (int r = 0; r < nRenderStates; r++)
            {
                auto rs = pRenderStates[r];

                LogFile::Format("    /* %08X */{ %s }\n", &statePtr->renderstates[r], VALS2STR(D3DRS_STRING(rs.type), D3DVAL2STR(0, rs.type, rs.value)));
            }
            LogFile::WriteLine("  }");
        }

        if (nT1States > 0)
        {
            LogFile::WriteLine("  TEXTURESTATES : {");
            for (int t = 0; t < nT1States; t++)
            {
                auto ts = pT1States[t];

                LogFile::Format("    /* %08X */{ %s }\n", &statePtr->texturestates[t], VALS2STR(D3DTSS_STRING(ts.type), D3DVAL2STR(1, ts.type, ts.value)));
            }
            LogFile::WriteLine("  }");
        }

        if (nT2States > 0)
        {
            LogFile::WriteLine("  SAMPLERSTATES : {");
            for (int s = 0; s < nT2States; s++)
            {
                auto ss = pT2States[s];

                LogFile::Format("    /* %08X */{ %s }\n", &statePtr->samplerstates[s], VALS2STR(D3DSAMP_STRING(ss.type), D3DVAL2STR(2, ss.type, ss.value)));
            }
            LogFile::WriteLine("  }");
        }

        LogFile::WriteLine("}");
    }

    void setGlobalStateParams(StateParamsHolder* pRenderStates, int nRenderStates,
        StateParamsHolder* pT1States, int nT1States,
        StateParamsHolder* pT2States, int nT2States)
    {
        dumpstateparams(-1, pRenderStates, nRenderStates, pT1States, nT1States, pT2States, nT2States);
        d3dStateManager_setGlobalStateParams(this, pRenderStates, nRenderStates, pT1States, nT1States, pT2States, nT2States);
    }

    static bool Install()
    {
        switch (gameversion)
        {
        case __DRIV3R_V100:
        {
            InstallCallback("d3dStateManager::SetGlobalStateParams",
                &setGlobalStateParams, {
                    cbHook<CALL>(0x5E6D01),
                }
            );
        } return true;
        default:
            HANDLER_CANNOT_BE_IMPLEMENTED_BECAUSE_PATCH_2_SUCKS();
            return false;
        }
    }
};

class renderer {
protected:
    static fiero::Field<0x44, d3dStateManager*> _Manager;
};

static auto renderer_registerStateParams = fiero::as<renderer>::func<void>(0x5E4680);
static fiero::Type<d3dStateManager*> pD3DStateManager(0x8AFAAC);

// lookup tables that will get filled with defaults if zero
static int g_FirstPassHandlers[52]{ 0 };
static int g_SecondPassHandlers[52]{ 0 };

/*
    NOT COMPATIBLE WITH PATCH 2
*/
class rendererHandler : public renderer {
public:
    void setStateParams(int* pStateIds, int nStateIds,
        StateParamsHolder* pRenderStates, int nRenderStates,
        StateParamsHolder* pT1States, int nT1States,
        StateParamsHolder* pT2States, int nT2States) {
        auto manager = _Manager.get(this);

        for (int i = 0; i < nStateIds; i++)
        {
            int state = pStateIds[i];

            // dump that shit
            manager->dumpstateparams(state, pRenderStates, nRenderStates, pT1States, nT1States, pT2States, nT2States);

            /* actually set the shit */
            d3dStateManager_SetRenderStateParams(manager, state, pRenderStates, nRenderStates);
            d3dStateManager_SetTextureStageStateParams(manager, state, pT1States, nT1States);
            d3dStateManager_SetSamplerStateParams(manager, state, pT2States, nT2States);
        }
    }

    void setStateParamsCustom(int state,
        StateParamsHolder* pRenderStates, int nRenderStates,
        StateParamsHolder* pTextureStates, int nTextureStates,
        StateParamsHolder* pSamplerStates, int nSamplerStates)
    {
        auto manager = _Manager.get(this);

        manager->dumpstateparams(state, pRenderStates, nRenderStates, pTextureStates, nTextureStates, pSamplerStates, nSamplerStates);

        if (nRenderStates > 0)
            d3dStateManager_SetRenderStateParams(manager, state, pRenderStates, nRenderStates);
        if (nTextureStates > 0)
            d3dStateManager_SetTextureStageStateParams(manager, state, pTextureStates, nTextureStates);
        if (nSamplerStates > 0)
            d3dStateManager_SetSamplerStateParams(manager, state, pSamplerStates, nSamplerStates);
    }

    // I really do mean INLINE!
    FORCEINLINE void setStateParamsCustom(int state,
        std::initializer_list<StateParamsHolder> renderStates,
        std::initializer_list<StateParamsHolder> textureStates = {},
        std::initializer_list<StateParamsHolder> samplerStates = {})
    {
        setStateParamsCustom(state,
            (StateParamsHolder*)renderStates.begin(), renderStates.size(),
            (StateParamsHolder*)textureStates.begin(), textureStates.size(),
            (StateParamsHolder*)samplerStates.begin(), samplerStates.size());
    }

    void applyFixupStateParams(void) {
        auto manager = _Manager.get(this);

        LogFile::WriteLine("Applying fixup state params...");

        // cleaner alpha textures
        setStateParamsCustom(3, {
            { D3DRS_ZFUNC,                          D3DCMP_LESSEQUAL },
            { D3DRS_ZWRITEENABLE,                   TRUE },
            { D3DRS_ALPHABLENDENABLE,               FALSE },
            { D3DRS_ALPHATESTENABLE,                TRUE },
            { D3DRS_CULLMODE,                       D3DCULL_CW },
            { D3DRS_ALPHAFUNC,                      D3DCMP_GREATER },
            { D3DRS_ALPHAREF,                       0x3F },
            { D3DRS_FOGENABLE,                      TRUE },
            { D3DRS_FOGTABLEMODE,                   D3DFOG_NONE },
            { D3DRS_SEPARATEALPHABLENDENABLE,       TRUE },
            { D3DRS_SPECULARENABLE,                 FALSE },
            }, {
                { D3DTSS_COLOROP,                       D3DTOP_MODULATE },
                { D3DTSS_COLORARG1,                     D3DTA_DIFFUSE },
                { D3DTSS_COLORARG2,                     D3DTA_TEXTURE },
                { D3DTSS_ALPHAOP,                       D3DTOP_MODULATE },
                { D3DTSS_ALPHAARG1,                     D3DTA_DIFFUSE },
                { D3DTSS_ALPHAARG2,                     D3DTA_TEXTURE },
            });

        // fixes steering wheels :)
        setStateParamsCustom(11, {
            { D3DRS_ZFUNC,                          D3DCMP_LESSEQUAL },
            { D3DRS_ZWRITEENABLE,                   TRUE },
            { D3DRS_ALPHATESTENABLE,                TRUE },
            { D3DRS_ALPHABLENDENABLE,               FALSE },
            { D3DRS_ALPHAFUNC,                      D3DCMP_GREATER },
            { D3DRS_ALPHAREF,                       0x9F },
            { D3DRS_FOGENABLE,                      FALSE },
            { D3DRS_FOGTABLEMODE,                   D3DFOG_NONE },
            { D3DRS_SPECULARENABLE,                 TRUE },
            }, {
                { D3DTSS_COLOROP,                       D3DTOP_MODULATE },
                { D3DTSS_COLORARG1,                     D3DTA_DIFFUSE },
                { D3DTSS_COLORARG2,                     D3DTA_TEXTURE },
                { D3DTSS_ALPHAOP,                       D3DTOP_DISABLE },
            }, {
                { D3DSAMP_MIPMAPLODBIAS,                -2.0f },
            });

        // lights (TODO: make these look sexy)
        setStateParamsCustom(19, {
            { D3DRS_ALPHABLENDENABLE,               TRUE },
            { D3DRS_ALPHATESTENABLE,                TRUE },
            { D3DRS_BLENDOP,                        D3DBLENDOP_ADD },
            { D3DRS_SRCBLEND,                       D3DBLEND_SRCALPHA },
            { D3DRS_DESTBLEND,                      D3DBLEND_ONE },
            }, {
                { D3DTSS_COLOROP,                       D3DTOP_MODULATE },
                { D3DTSS_COLORARG1,                     D3DTA_DIFFUSE },
                { D3DTSS_COLORARG2,                     D3DTA_TEXTURE },
                { D3DTSS_ALPHAOP,                       D3DTOP_MODULATE },
                { D3DTSS_ALPHAARG1,                     D3DTA_DIFFUSE },
                { D3DTSS_ALPHAARG2,                     D3DTA_TEXTURE },

            });

        // finally fixing the shadows!
        setStateParamsCustom(39, {
            { D3DRS_ZFUNC,                          D3DCMP_LESSEQUAL },
            { D3DRS_ZWRITEENABLE,                   FALSE },
            { D3DRS_ALPHATESTENABLE,                TRUE },
            { D3DRS_ALPHABLENDENABLE,               TRUE },
            { D3DRS_ALPHAREF,                       0x3F },
            { D3DRS_FOGENABLE,                      FALSE },
            { D3DRS_SRCBLEND,                       D3DBLEND_SRCALPHA },
            { D3DRS_DESTBLEND,                      D3DBLEND_ONE },
            { D3DRS_STENCILREF,                     0x01 },
            { D3DRS_STENCILPASS,                    D3DSTENCILOP_REPLACE },
            { D3DRS_STENCILFUNC,                    D3DCMP_ALWAYS },
            { D3DRS_STENCILFAIL,                    D3DSTENCILOP_KEEP },
            { D3DRS_STENCILENABLE,                  TRUE },
            { D3DRS_COLORWRITEENABLE,               0x08 },
            }, {
                { D3DTSS_COLOROP,                       D3DTOP_MODULATE },
                { D3DTSS_COLORARG1,                     D3DTA_DIFFUSE },
                { D3DTSS_COLORARG2,                     D3DTA_TEXTURE },
                { D3DTSS_ALPHAOP,                       D3DTOP_BLENDTEXTUREALPHA },
            }, {
                { D3DSAMP_ADDRESSU,                     D3DTADDRESS_CLAMP },
                { D3DSAMP_ADDRESSV,                     D3DTADDRESS_CLAMP },
            });

        // quick and dirty way to try fixing mipmaps
        manager->globalstate.addSamplerState({ D3DSAMP_MIPMAPLODBIAS, -1.0f });
    }

    void registerStateParams(void) {
        auto manager = _Manager.get(this);

        LogFile::Format("Initializing renderer state params (manager: %08X)\n", manager);
        renderer_registerStateParams(this);

        applyFixupStateParams();
    }

    // ripped directly from Patch 2
    // I fail to see a difference when using this, however
    static int GetDefaultFirstPassStateHandlerIndex_PATCH2(int type) {
        switch (type)
        {
        case 0:
        case 21:
            return 4;
        case 1:
        case 9:
        case 13:
            return 30;
        case 2:
            return 32;
        case 3:
            return 31;
        case 4:
            return 39;
        case 5:
            return 33;
        case 6:
            return 2;
        case 7:
        case 16:
        case 17:
        case 18:
        case 29:
            return 12;
        case 10:
            return 14;
        case 11:
        case 12:
            return 10;
        case 14:
            return 15;
        case 15:
            return 16;
        case 19:
        case 23:
            return 9;
        case 20:
            return 7;
        case 22:
            return 5;
        case 24:
        case 28:
            return 8;
        case 25:
            return 21;
        case 26:
            return 38;
        case 27:
            return 17;
        case 30:
            return 19;
        case 31:
        case 32:
            return 13;
        case 33:
        case 47:
        case 48:
        case 49:
        case 50:
        case 51:
        case 53:
            return 23;
        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
            return 25;
        case 46:
            return 24;
        case 52:
            return 26;
        }

        return 1;
    }

    static int GetDefaultFirstPassStateHandlerIndex(int type) {
        switch (type)
        {
            //
            // FIXUPS
            //
        case 8:
            return 3;
        case 11:
            return 11;
            //
            // DEFAULTS
            //
        case 0:
        case 21:
            return 4;
        case 1:
        case 9:
        case 13:
            return 30;
        case 2:
            return 32;
        case 3:
            return 31;
        case 4:
            return 39;
        case 5:
            return 33;
        case 6:
            return 2;
        case 7:
        case 16:
        case 17:
        case 18:
        case 28:
            return 12;
        case 10:
            return 14;
        case 12:
            return 10;
        case 14:
            return 15;
        case 15:
            return 16;
        case 19:
        case 23:
            return 9;
        case 20:
            return 7;
        case 22:
            return 5;
        case 24:
        case 27:
            return 8;
        case 25:
            return 21;
        case 26:
            return 17;
        case 29:
            return 19;
        case 30:
        case 31:
            return 13;
        case 32:
        case 46:
        case 47:
        case 48:
        case 49:
        case 50:
        case 52:
            return 23;
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
            return 25;
        case 45:
            return 24;
        case 51:
            return 26;
        }
        return 1;
    }

    static int GetDefaultSecondPassStateHandlerIndex(int type) {
        switch (type)
        {
        case 3:
        case 4:
            return 17;
        case 5:
            return 18;
        case 6:
            return 27;
        case 7:
            return 28;
        case 8:
            return 40;
        case 9:
            return 6;
        case 10:
            return 20;
        case 11:
            return 22;
        }
        return 0;
    }

    int GetFirstPassStateHandlerIndex(int type) {
        auto handler = &g_FirstPassHandlers[type];

        if (*handler == 0)
            *handler = GetDefaultFirstPassStateHandlerIndex(type);

        return *handler;
    }

    int GetSecondPassStateHandlerIndex(int type) {
        auto handler = &g_SecondPassHandlers[type];

        // this will constantly get set if the default state is zero
        if (*handler == 0)
            *handler = GetDefaultSecondPassStateHandlerIndex(type);

        return *handler;
    }

    int SetFirstPassSamplerStates(int type, int /* state */) {
        // TODO
    }

    int SetSecondPassRenderState(int type, int state) {
        return fiero::as<d3dStateManager>::func<int, int, int>(0x5DFDA0)(pD3DStateManager, type, state);
    }

    int SetSecondPassTextureStageState(int type, int state) {
        return fiero::as<d3dStateManager>::func<int, int, int>(0x5DFF20)(pD3DStateManager, type, state);
    }

    int SetSecondPassSamplerState(int type, int state) {
        return fiero::as<d3dStateManager>::func<int, int, int>(0x5DFE50)(pD3DStateManager, type, state);
    }

#ifdef USE_GUI
    HRESULT Present(long hwndOverride) {
        auto result = fiero::as<renderer>::func<HRESULT, long>(0x5DA840)(this, hwndOverride);
#if DRAW_GUI
        if (result == D3DERR_DEVICELOST) {
            if (pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
                gui::Reset();
        }
#endif
        return result;
    }

    void Draw() {
        // let the game go first
        fiero::as<renderer>::func<void>(0x5DA660)(this);

        // now we draw our gui :)
        DrawGUI();
    }
#endif

    static bool Install() {
        if (gameversion != __DRIV3R_V100) {
            HANDLER_CANNOT_BE_IMPLEMENTED_BECAUSE_PATCH_2_SUCKS();
            return false;
        }

        InstallCallback("renderer::registerStateParams",
            &registerStateParams, {
                cbHook<CALL>(0x5DAF65),
            }
        );

        InstallCallback("renderer::setStateParams",
            &setStateParams, {
                cbHook<JMP>(0x5D7210),
            }
        );

        InstallCallback("renderer::GetFirstPassStateHandlerIndex",
            &GetFirstPassStateHandlerIndex, {
                cbHook<CALL>(0x5DF29D),
            }
        );

        InstallCallback("renderer::GetSecondPassStateHandlerIndex",
            &GetSecondPassStateHandlerIndex, {
                cbHook<CALL>(0x5DF2EF),
            }
        );
#ifdef USE_GUI
        InstallCallback("renderer::present",
            &Present, {
                cbHook<CALL>(0x5B4132),
            }
        );

        InstallCallback("renderer::draw",
            &Draw, {
                cbHook<CALL>(0x5B4197),
            }
        );
#endif
        /*
            implement patch 2 behavior;
            call the d3dStateManager directly, bypassing the 'type < 41' check
        */

        // the game doesn't like this very much
        //--InstallCallback("renderer::SetSecondPassRenderState",
        //--    &SetSecondPassRenderState, {
        //--        cbHook<CALL>(0x5DF30F)
        //--    }
        //--);
        //--
        //--InstallCallback("renderer::SetSecondPassTextureStageState",
        //--    &SetSecondPassTextureStageState, {
        //--        cbHook<CALL>(0x5DF31E)
        //--    }
        //--);
        //--
        //--InstallCallback("renderer::SetSecondPassSamplerState",
        //--    &SetSecondPassSamplerState, {
        //--        cbHook<CALL>(0x5DF32D)
        //--    }
        //--);

        return true;
    }
};

intptr_t fnFrameworkRun;

class FrameworkHandler : public IFramework {
public:
    void Run() {
        bool subclassWindow = true;
        bool canUseGui = false;

        auto hwnd = hamster::GetMainWindow();

        if (subclassWindow)
        {
            LogFile::Write("Subclassing window...");

            if (SubclassGameWindow(hwnd, WndProcNew, &hProcOld))
            {
                LogFile::Write("Done!\n");
                canUseGui = true;
            }
            else {
                LogFile::Write("FAIL!\n");
            }
        }

        //pD3D = hamster::GetD3D();
        pD3DDevice = hamster::GetD3DDevice();

        if (pD3DDevice != NULL)
        {
            LOG("Successfully retrieved the D3D device pointer!");

            pD3DDeviceHook = new IDirect3DDevice9Hook;
            hamster::SetD3DDevice(pD3DDeviceHook);
        }
        else
        {
            LOG("Could not retrieve the D3D device pointer!");
            canUseGui = false;
        }

        pUserCmdProxy = hamster::UserCommandProxy::Get();

        if (pUserCmdProxy != NULL)
        {
            pUserCmdProxyHook = AutoHook::Create<IUserCommandProxyHook>(pUserCmdProxy);
            hamster::UserCommandProxy::Set(pUserCmdProxyHook);

            LOG("Successfully hooked into the user command proxy!");
        }
        else
        {
            LOG("Could not retrieve the user command proxy!");
        }

#ifdef USE_GUI
        // if we subclassed the window and got the device
        if (canUseGui)
            g_bGUIWaiting = true;
#endif

        LogFile::AppendLine();

        /*
        if (gameversion == __DRIV3R_V120)
        {
            LOG("Loading menu from memory...");

            HWND gameHwnd = hamster::GetMainWindowHwnd();
            HMENU hMenu = LoadMenuIndirect((MENUTEMPLATE*)0x8E6BA0);

            if (hMenu != NULL && IsMenu(hMenu)) {
                LOG("Successfully loaded menu from memory!");

                //if (SetMenu(gameHwnd, hMenu) != NULL) {
                //    LOG("Successfully set the menu!");
                //} else {
                //    LOG("FAILED to set the menu!");
                //}
            }
            else {
                LOG("FAILED to load menu from memory!");
            }
        }
        */

        reinterpret_cast<MemberCall<void, IFramework> &>(fnFrameworkRun)(this);
    };

    static bool Install()
    {
        InstallCallback("Hooks into the game framework.", &Run, addressof<CALL>(0x5B7B2C, 0x576E0B), &fnFrameworkRun);

        if (gameversion == __DRIV3R_V120) {
            // DUDE, THIS FUCKING GAME
            InstallPatch({
                0xFF, 0x05, 0x44, 0xFA, 0x8A, 0x00,     // inc g_bGameRunning (saves us the 1 byte we lost)
                0x8B, 0x0D, 0x58, 0xC2, 0x8A, 0x00,     // mov ecx, pFramework
                }, { 0x576DFF });

            InstallPatch({
                0x8B, 0xF9,                             // mov edi, ecx
                }, { 0x571D84 });
        }

        return true;
    }
};

// TODO: make this configurable
static bool g_bIsWindowed = false;

static BOOL WindowUpdated(HWND hWnd, int width, int height, bool repaint) {
    if (g_bIsWindowed)
    {
        HDC hDC = GetDC(NULL);
        int screenWidth = GetDeviceCaps(hDC, HORZRES);
        int screenHeight = GetDeviceCaps(hDC, VERTRES);
        ReleaseDC(0, hDC);

        int x = (screenWidth - width) / 2;
        int y = (screenHeight - height) / 2;

        int captionHeight = GetSystemMetrics(SM_CYCAPTION);

        return MoveWindow(hWnd, x, y, width, height + captionHeight, repaint);
    }

    return FALSE;
}

static BOOL WindowUpdated(HWND hWnd) {
    if (g_bIsWindowed)
    {
        RECT rect;
        GetWindowRect(hWnd, &rect);

        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        LogFile::Format("Updating window (%dx%d)\n", width, height);
        return WindowUpdated(hWnd, width, height, false);
    }

    return FALSE;
}

LRESULT APIENTRY WndProcNew(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef USE_GUI
    if (g_bGUIReady
        && g_bGUIFocus
        && gui::runtime::WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;
#endif
    switch (uMsg) {
    case WM_DESTROY:
#ifdef USE_GUI
        if (g_bGUIReady) {
            gui::Shutdown();
            g_bGUIReady = false;
        }
#endif
        break;
#ifdef USE_GUI
    case WM_SIZE:
        if (g_bGUIReady)
            gui::Reset();
        break;
    
    case WM_SYSCOMMAND:
        if (g_bGUIReady && g_bGUIFocus)
        {
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
        }
        break;
#endif
    case WM_KEYUP:
        if (HandleKeyPress((DWORD)wParam))
            return 0;
        break;
    }

    return CallWindowProc(hProcOld, hWnd, uMsg, wParam, lParam);
}

class HamsterViewport {
protected:
    int         MultiSampleType;
    HRESULT     MultiSampleQuality;
    char        field_8;
    bool        AntiAliasingEnabled;
    int         TextureFilter;
    char        field_10;
    bool        DriverManagementDisabled;
    UINT        Adapter;
    D3DCAPS9    D3DCaps;
    HWND        Hwnd;
    bool        Windowed;
    int         ColorDepth;
    IDirect3D9* IDirect3D9;
    IDirect3DDevice9
        * IDirect3DDevice9;
    D3DDEVTYPE  D3DDevType;
    D3DPRESENT_PARAMETERS
        D3DPresentParameters;
    int         FocusWindow;
};

intptr_t cbUpdateDevices;
intptr_t cbUpdateWindow;
intptr_t cbSetWindowSize;

/*
    NOT COMPATIBLE WITH PATCH 2
*/
class HamsterViewportHandler : public HamsterViewport {
public:
    HRESULT UpdateDevices() {
        if (this->Windowed)
            WindowUpdated(this->Hwnd);

        return fiero::as<HamsterViewport>::func<HRESULT>(cbUpdateDevices)(this);
    }

    static bool Install() {
        if (gameversion != __DRIV3R_V100) {
            HANDLER_CANNOT_BE_IMPLEMENTED_BECAUSE_PATCH_2_SUCKS();
            return false;
        }

        InstallCallback("Updates the window upon initialization.",
            &UpdateDevices, cbHook<CALL>(0x5AF06C), &cbUpdateDevices);

        return true;
    }
};

class WindowedModeHandler {
public:
    bool SetActiveDisplayMode(void* params) {
        if (g_bIsWindowed)
        {
            fiero::Type<HWND> hWnd(0x7F8340);

            struct display_settings {
                int display_bpp;
                wchar_t display_res_text[32];
                int display_res_width;
                int display_res_height;
                int display_refresh_rate;
                int display_format;
            };

            auto info = reinterpret_cast<display_settings*>(params);

            WindowUpdated(hWnd, info->display_res_width, info->display_res_height, true);
        }

        return true;
    }

    static bool Install() {
        if (gameversion != __DRIV3R_V100) {
            HANDLER_CANNOT_BE_IMPLEMENTED_BECAUSE_PATCH_2_SUCKS();
            return false;
        }

        InstallCallback("Updates the window when the display mode has changed.",
            &SetActiveDisplayMode, {
                cbHook<JMP>(0x57C756),
            }
        );

        // windowed mode! :D
        mem::write<uint32_t>(0x5B799F + 1, WS_POPUP | WS_CAPTION | WS_SYSMENU);

        // initialize/update in windowed mode
        mem::write<uint8_t>(0x5A5F03 + 1, 1);
        mem::write<uint8_t>(0x5AEFD7 + 4, 1);

        // *sigh*
        mem::write<uint8_t, uint8_t>(0x5A5E85, 0x90, 0x90);
        mem::write<uint8_t, uint8_t>(0x5A5EE0, 0x90, 0x90);
        mem::write<uint8_t, uint8_t>(0x5D4A10, 0x90, 0x90);
        mem::write<uint8_t, uint8_t>(0x5D4A1C, 0x90, 0x90);

        // TODO: make this configurable
        g_bIsWindowed = true;

        return true;
    }
};

class PatchHandler {
protected:
    static void Install_V100() {
        // fix the damn z-fighting!!!
        mem::write(0x5A9B40, -1.3275f);

        // fix shiny tanner!
        // add a special jump case for shader type 33
        // put it just past the jump lookup table ;)
        mem::write<uint32_t>(0x5EC624, 0x5EC5CA);

        // the jump table is at 0x5EC5F0, and our special case is at 0x5EC624
        // therefore, we need an index of 13
        mem::write<uint8_t>(0x5EC600 + 33, 13);
    }

    static void Install_V120() {

    }

    static void InstallShared() {
        // set a minimum display resolution
        static const int m_width = 1024;
        static const int m_height = 768;

        // height
        mem::write({
            addressof(0x57C5EA + 1, 0x54A5AA + 1),
            addressof(0x5AEF23 + 1, 0x57F66F + 1),
            addressof(0x5AEFA1 + 4, 0x57F6E7 + 4),
            addressof(0x5B7993 + 1, 0x576C6B + 1),
            addressof(0x5D46C8 + 1, 0x5C9292 + 1),
            }, m_height);

        // width
        mem::write({
            addressof(0x57C5EF + 1, 0x54A5AF + 1),
            addressof(0x5AEF28 + 1, 0x57F674 + 1),
            addressof(0x5AEF99 + 4, 0x57F6DF + 4),
            addressof(0x5B7998 + 1, 0x576C70 + 1),
            addressof(0x5D46CD + 1, 0x5C9297 + 1),
            }, m_width);

        // fixes shiny wheels/interiors
        // ...but in patch 2, EVERYTHING IS SHINY AT NIGHT!?
        mem::write<uint8_t>(addressof(0x5EC600 + 28, 0x5E1058 + 28), 1);

        // turn off the high beams :P
        mem::write(addressof(0x7E261C, 0x7E5F00), 90.0f);

        // fixes shiny wheels/interiors
        // ...but in patch 2, EVERYTHING IS SHINY AT NIGHT!?
        mem::write<uint8_t>(addressof(0x5EC600 + 28, 0x5E1058 + 28), 1);

        // turn off the high beams :P
        mem::write(addressof(0x7E261C, 0x7E5F00), 90.0f);
    }
public:
    static bool Install() {
        LogFile::WriteLine("[CompatHandler] Installing shared patches...");
        InstallShared();

        switch (gameversion) {
        case __DRIV3R_V100:
            LogFile::WriteLine("[CompatHandler] Installing vanilla patches...");
            Install_V100();
            break;
        case __DRIV3R_V120:
            LogFile::WriteLine("[CompatHandler] Installing patch 2 patches...");
            Install_V120();
            break;
        }

        return true;
    }
};

static intptr_t _WinMainCallback;

class HookSystemHandler
{
    static void InstallHandlers() {
        LogFile::WriteLine("Installing handlers...");

        /*
            Initialize the important handlers first
        */

        InstallHandler<FrameworkHandler>("Main framework");
        InstallHandler<PatchHandler>("Patching framework");
        InstallHandler<WindowedModeHandler>("Windowed mode");

        /*
            Now install everything else
        */

        InstallHandler<rendererHandler>("renderer");
        InstallHandler<d3dStateManager>("d3dStateManager");
        InstallHandler<effectManagerHandler>("effectManager");
        InstallHandler<DrawListHandler>("DrawList");

        InstallHandler<HamsterViewportHandler>("HamsterViewport");

        init_base::RunAll();
    }

    static void InstallPatches() {
        LogFile::WriteLine("Installing patches...");

        switch (gameversion)
        {
        case __DRIV3R_V100:
        {

        } break;
        case __DRIV3R_V120:
        {

        } break;
        }
    }
protected:
    // this fucking game, man....
    static int __stdcall Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
        LogFile::WriteLine("Initializing the hook framework...");

        // initialize game module info
        if (!fxModule::Initialize(mymodule, hInstance))
            LogFile::WriteLine("Couldn't initialize the game's module info!");

        InstallPatches();
        InstallHandlers();

        LogFile::WriteLine("Returning control to the game...");
        return reinterpret_cast<decltype(WinMain)*>(_WinMainCallback)(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
    }
public:
    static bool Install() {
        InstallCallback("Allows the hook to initialize before the game runs.",
            &Initialize, addressof<CALL>(0x5F053C, 0x5E5B04), &_WinMainCallback);

        /*
            IMPORTANT:
            Add any patches/callbacks here that must be initialized prior to the game's entry point.
            This should be used for very very advanced callbacks/patches only!
        */

        return true;
    }
};

//
// Initialize all the important stuff prior to Driv3r starting up
//
void Initialize(RSDSEntry& gameInfo) {
    gameversion = gameInfo.version;

    // initialize game manager
    pDriv3r = new CDriv3r(gameversion);

    HookSystemHandler::Install();
}

#ifdef USE_GUI
//
// gui user-functions
//

static char g_MegaBuffer[4096 * 4096 * 64] = { NULL };
static size_t g_MegaBufferOffset = 0;

void *HungryAllocate(size_t size, void *user_data) {
    if (g_MegaBufferOffset >= sizeof(g_MegaBuffer))
        return nullptr;

    void *buffer = (void *)g_MegaBuffer[g_MegaBufferOffset];
    g_MegaBufferOffset += size;

    ZeroMemory(buffer, size);

    return buffer;
}

void HungryFree(void *ptr, void *user_data) {
    // LOL
}

void gui::Initialize(HWND hwnd, IDirect3DDevice9 *device)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();

    ImGui::SetAllocatorFunctions(HungryAllocate, HungryFree);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    gui::runtime::Initialize(hwnd);
    gui::device::Initialize(device);
}

void gui::Shutdown()
{
    gui::device::Shutdown();
    gui::runtime::Shutdown();

    ImGui::DestroyContext();
}

void gui::Update()
{
    // Our state
    static bool show_demo_window = true;
    static bool show_another_window = false;

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }
}
#endif