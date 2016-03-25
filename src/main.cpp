#include "main.h"

CDriv3r                 *pDriv3r = 0;

IDirect3D9              *pD3D;
IDirect3DDevice9        *pD3DDevice;

IDirect3D9Hook          *pD3DHook;
IDirect3DDevice9Hook    *pD3DDeviceHook;

IUserCommandProxy       *pUserCmdProxy;
IUserCommandProxyHook   *pUserCmdProxyHook;

WNDPROC hProcOld;
LRESULT APIENTRY WndProcNew(HWND, UINT, WPARAM, LPARAM);

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
#pragma comment(linker, "/EXPORT:DirectInput8Create=_DirectInput8Create_Impl")
HRESULT NAKED DirectInput8Create_Impl(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter)
{
    _asm jmp DWORD PTR ds:lpDI8Create
}

bool SubclassGameWindow(HWND gameWnd, WNDPROC pWndProcNew, WNDPROC *ppWndProcOld)
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

bool HandleKeyPress(DWORD vKey)
{
    switch (vKey) {
        case VK_OEM_3:
            // close the game!
            ExitProcess(EXIT_SUCCESS);
            return true;
        case VK_F5:
        {
            LOG("Testing the file chunker hook...");
            LOG(" - Creating new chunker...");
            CFileChunkerHook fileChunker = CFileChunkerHook();

            LogFile::Format(" - Chunk count: %d\n", fileChunker.GetChunkCount());

            LOG(" - Testing complete!");
            return true;
        }
        case VK_F6:
        {
            LOG("Dumping shaders...");

            const int gameVersion = pDriv3r->GetGameVersion();
            char shadersDir[MAX_PATH] = { NULL };
            char *appPath = hook::VTHook(pDriv3r->GetSingletonPointer(D3_SOBJ_SYSTEMCONFIG))[16].Call<char*>();

            char *shadersVer = (gameVersion == __DRIV3R_V100) ? "v100" : "v120";

            sprintf(shadersDir, "%s\\shaders\\%s", appPath, shadersVer);

            SHCreateDirectoryEx(NULL, shadersDir, NULL);

            struct _D3_SHADER_EXPORT {
                const char* fmtStr;
                const _D3_SHADER *shaders;
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
            //--CFileChunkerHook shaderChunker;
            //--int shaderIndex = 0;
            //--
            //--for (int i = 0; i < sizeof(CFileChunkerHook); i += 4)
            //--    *(DWORD*)((BYTE*)&shaderChunker + i) = 0xDEC015A1;
            //--
            //--shaderChunker = CFileChunkerHook();

            for (auto out_ : outShaders)
            {
                char outDir[MAX_PATH];
                sprintf(outDir, out_.fmtStr, &shadersDir);
            
                SHCreateDirectoryEx(NULL, outDir, NULL);
            
                for (int i = 0; i < out_.nShaders; i++)
                {
                    auto shader = out_.shaders[i];
            
                    auto offset = shader.info[gameVersion].offset;
                    auto size = shader.info[gameVersion].size;
            
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

                        FILE *outFile = fopen(outPath, "wb");

                        if (shader.type != -1)
                        {
                            if (D3DXCreateEffect(pDriv3r->GetD3DDevice(), (LPCVOID)offset, size, NULL, NULL, D3DXSHADER_NO_PRESHADER, NULL, &pEffect, &outBuffer) == D3D_OK)
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

                        FILE *outFile = fopen(outPath, "wb");

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
            //--int outSize = sizeof(CFileChunkerHook);
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
        case 'v':
        case 'V':
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
            return true;
    }

    return false;
}

LRESULT APIENTRY WndProcNew(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //const DWORD *ptr = _PTR(0x8AC278);

    //const DWORD *ptr = pDriv3r->GetSingletonObjectPointer(18);

    switch (uMsg) {
        case WM_DESTROY:
        {
            /*
            wParam
                This parameter is not used.
            lParam
                This parameter is not used.
            */

            hook::VTHook(pDriv3r->GetSingletonPointer(D3_SOBJ_RENDERER))[32]();
            return 0;
        }

        case WM_KEYUP:
        {
            if (HandleKeyPress((DWORD)wParam))
                return 0;
        } break;

        //case WM_MOVE:
        //{
        //    /*
        //    wParam
        //        This parameter is not used.
        //    lParam
        //        The x and y coordinates of the upper-left corner of the client area of the window.
        //
        //        LOWORD: Contains the x-coordinate.
        //        HIWORD: Contains the y-coordinate.
        //    */
        //
        //    if (*(bool*)((pDriv3r->GetGameVersion() == __DRIV3R_V100) ? 0x8B86F0 : 0x8AFA44))
        //    {
        //        int xPos = GET_X_LPARAM(lParam);
        //        int yPos = GET_Y_LPARAM(lParam);
        //
        //        float *wndX = (float*)(*ptr + 0x14);
        //        float *wndY = (float*)(*ptr + 0x18);
        //        
        //        *wndX = (float)xPos;
        //        *wndY = (float)yPos;
        //
        //        __asm {
        //            push [wndY]
        //            push [wndX]
        //        }
        //
        //        //hook::ThisCall(0x8AC238, 0x84);
        //        hook::ThisCall(pDriv3r->GetPointer(D3_SOBJ_RENDERER), 0x84);
        //
        //        char buf[20]; // 'window_size %d %d'
        //        _snprintf(buf, sizeof(buf), "window_size %d %d", xPos, yPos);
        //
        //        pUserCmdProxy->IssueCommand(buf, NULL);
        //    }
        //
        //    return 0;
        //}

        //case WM_SETREDRAW:
        //    /*
        //    wParam
        //        The redraw state. If this parameter is TRUE, the content can be redrawn after a change. If this parameter is FALSE, the content cannot be redrawn after a change.
        //    lParam
        //        This parameter is not used.
        //    */
        //    return 0;

        //case WM_SETCURSOR:
        //{
        //    /*
        //    wParam
        //        A handle to the window that contains the cursor.
        //    lParam
        //        LOWORD: Specifies the hit-test code.
        //        HIWORD: Specifies the identifier of the mouse message.
        //    */
        //    if ((HWND)wParam == hWnd)
        //    {
        //        if (!(*(bool*)(*ptr + 0x8)) && (LOWORD(lParam) == HTCLIENT))
        //        {
        //            SetCursor(0);
        //            return 1;
        //        }
        //    }
        //} goto HANDLE_MSG;

        //case WM_MOUSEWHEEL:
        //{
        //    /*
        //    wParam
        //        LOWORD: Indicates whether various virtual keys are down.
        //        HIWORD: Indicates the distance the wheel is rotated.
        //    lParam
        //        Cursor coordinates relative to upper-left of screen.
        //
        //        LOWORD: Specifies the x-coordinate of the cursor.
        //        HIWORD: Specifies the y-coordinate of the cursor.
        //    */
        //
        //    int xPos = GET_X_LPARAM(lParam);
        //    int yPos = GET_Y_LPARAM(lParam);
        //
        //    ScreenToClient(hWnd, (POINT*)lParam);
        //
        //    float *wndX = (float*)(*ptr + 0x14);
        //    float *wndY = (float*)(*ptr + 0x18);
        //
        //    float x, y;
        //
        //    int w1 = (wParam >> 2) & 1;
        //    int w2 = (wParam >> 3) & 1;
        //
        //    __asm {
        //        fild xPos
        //        fdiv [wndX]
        //        fstp x
        //
        //        fild yPos
        //        fdiv [wndY]
        //        fstp y
        //
        //        push y
        //        push x
        //        push w2
        //        push w1
        //
        //        //mov ecx, __DWPTR_DS(0x8AC270)
        //        //mov esi, [ecx]
        //        //
        //        //call __DWPTR(esi + 48h);
        //    }
        //
        //    //hook::ThisCall(0x8AC270, 0x48);
        //    hook::ThisCall(pDriv3r->GetPointer(D3_SOBJ_MOUSECONTROL), 0x48);
        //
        //    return 0;
        //}

        // Handle click events
        //case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_LBUTTONDBLCLK:
        //case WM_RBUTTONDOWN: case WM_RBUTTONUP: case WM_RBUTTONDBLCLK:
        //case WM_MBUTTONDOWN: case WM_MBUTTONUP: case WM_MBUTTONDBLCLK:
        //{
        //    /*
        //    wParam
        //        Indicates whether various virtual keys are down.
        //    lParam
        //        Cursor coordinates relative to upper-left of screen.
        //
        //        LOWORD: Specifies the x-coordinate of the cursor.
        //        HIWORD: Specifies the y-coordinate of the cursor.
        //    */
        //
        //    int xPos = GET_X_LPARAM(lParam);
        //    int yPos = GET_Y_LPARAM(lParam);
        //
        //    float wndX = *(float*)(*ptr + 0x14);
        //    float wndY = *(float*)(*ptr + 0x18);
        //
        //    float x, y;
        //
        //    int w1 = (wParam >> 2) & 1;
        //    int w2 = (wParam >> 3) & 1;
        //
        //    __asm {
        //        fild xPos
        //        fdiv [wndX]
        //        fstp x
        //
        //        fild yPos
        //        fdiv [wndY]
        //        fstp y
        //
        //        push y
        //        push x
        //        push w2
        //        push w1
        //
        //        //mov ecx, __DWPTR_DS(0x8AC270)
        //        //mov esi, [ecx]
        //
        //        // ==== CALL VALUES ===============
        //        // LBUTTONDOWN:    20h < 0  ; 0x201
        //        // LBUTTONUP:      24h < 1  ; 0x202
        //        // LBUTTONDBLCLK:  28h < 2  ; 0x203
        //        // 
        //        // RBUTTONDOWN:    38h < 3  ; 0x204
        //        // RBUTTONUP:      3Ch < 4  ; 0x205
        //        // RBUTTONDBLCLK:  40h < 5  ; 0x206
        //        //
        //        // MBUTTONDOWN:    2Ch < 6  ; 0x207
        //        // MBUTTONUP:      30h < 7  ; 0x208
        //        // MBUTTONDBLCLK:  34h < 8  ; 0x209
        //        // ================================
        //    }
        //
        //    hook::ThisCall(pDriv3r->GetPointer(D3_SOBJ_MOUSECONTROL), ((uMsg - WM_LBUTTONDOWN) << 2) + 0x20);
        //    
        //    //LPVOID proc = pInputHook->GetMemberAddress(0x20 + ((uMsg - WM_LBUTTONDOWN) * 4));
        //    //
        //    //((void(__thiscall *)(THIS_ void*, int, int, POINTFLOAT))proc)(pInputHook, w1, w2, pos);
        //    return 0;
        //}

        //case WM_MOUSEMOVE:
        //{
        //    /*
        //    wParam
        //        LOWORD: Indicates whether various virtual keys are down.
        //        HIWORD: Unused.
        //    lParam
        //        LOWORD: The x-coordinate of the pointer, relative to the upper-left corner of the screen.
        //        HIWORD: The y-coordinate of the pointer, relative to the upper-left corner of the screen.
        //    */
        //
        //    int xPos = GET_X_LPARAM(lParam);
        //    int yPos = GET_Y_LPARAM(lParam);
        //
        //    int *curX = (int*)(*ptr + 0x0C);
        //    int *curY = (int*)(*ptr + 0x10);
        //
        //    if (xPos != *curX || yPos != *curX)
        //    {
        //        float *wndX = (float*)(*ptr + 0x14);
        //        float *wndY = (float*)(*ptr + 0x18);
        //
        //        float x, y;
        //
        //        int w1 = (wParam >> 2) & 1;
        //        int w2 = (wParam >> 3) & 1;
        //
        //        __asm {
        //            fild xPos
        //            fdiv [wndX]
        //            fstp x
        //
        //            fild yPos
        //            fdiv [wndY]
        //            fstp y
        //
        //            push y
        //            push x
        //            push w2
        //            push w1
        //
        //            //mov ecx, __DWPTR_DS(0x8AC270)
        //            //mov eax, [ecx]
        //            //
        //            //call __DWPTR(eax + 44h)
        //            //
        //            //mov curX, ebx
        //            //mov curY, edi
        //        }
        //
        //        hook::ThisCall(pDriv3r->GetPointer(D3_SOBJ_MOUSECONTROL), 0x44);
        //
        //        *curX = xPos;
        //        *curY = yPos;
        //
        //        return 0;
        //    }
        //} goto HANDLE_MSG;

        //HANDLE_MSG:
        //    return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return CallWindowProc(hProcOld, hWnd, uMsg, wParam, lParam);
}

void SetupHooks() {
    bool subclassWindow = true;

    if (subclassWindow)
    {
        LogFile::Write("Subclassing window...");

        if (SubclassGameWindow(pDriv3r->GetMainWindowHwnd(), WndProcNew, &hProcOld))
            LogFile::Write("Done!\n");
        else
            LogFile::Write("FAIL!\n");
    }

    //pD3D = pDriv3r->GetD3D();
    pD3DDevice = pDriv3r->GetD3DDevice();

    if (pD3DDevice != NULL)
    {
        LOG("Successfully retrieved the D3D device pointer!");

        //pD3DDeviceHook = new IDirect3DDevice9Hook;
        //pDriv3r->SetD3DDevice(pD3DDeviceHook);
    }
    else
    {
        LOG("Could not retreive the D3D device pointer!");
    }

    pUserCmdProxy = pDriv3r->GetUserCommandProxy();
    
    if (pUserCmdProxy != NULL)
    {
        pUserCmdProxyHook = new IUserCommandProxyHook(pUserCmdProxy);

        if (pDriv3r->SetUserCommandProxy(pUserCmdProxyHook))
        {
            LOG("Successfully hooked into the user command proxy!");
        }
        else
        {
            LOG("Could not hook into the user command proxy!");
        }
    }
    else
    {
        LOG("Could not retrieve the user command proxy!");
    }

    LogFile::AppendLine();

    /*
    if (pDriv3r->GetGameVersion() == __DRIV3R_V120)
    {
        LOG("Loading menu from memory...");

        HWND gameHwnd = pDriv3r->GetMainWindowHwnd();
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

    //pD3DHook = new IDirect3D9Hook;
    //pD3DDeviceHook = new IDirect3DDevice9Hook;

    //pDriv3r->SetD3D((DWORD)pD3DHook);
    //LOG("pDriv3r->SetD3DDevice");
    //pDriv3r->SetD3DDevice(pD3DDeviceHook);

    //pInputHook = new IVTableHook(0x8AC270);
}

void InstallVTHook(DWORD dwHookAddr, DWORD dwNewAddr) {
    DWORD flOldProtect;

    VirtualProtect((LPVOID)dwHookAddr, 4, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *(DWORD*)dwHookAddr = dwNewAddr;
    VirtualProtect((LPVOID)dwHookAddr, 4, flOldProtect, &flOldProtect);
}

void InstallVTHook(DWORD dwHookAddr, DWORD dwNewAddr, LPDWORD lpdwOldAddr) {
    *lpdwOldAddr = *(DWORD*)dwHookAddr;
    InstallVTHook(dwHookAddr, dwNewAddr);
}

void InstallPatch(DWORD dwAddress, BYTE *patchData, DWORD dwSize) {
    DWORD flOldProtect;

    VirtualProtect((LPVOID)dwAddress, dwSize, PAGE_EXECUTE_READWRITE, &flOldProtect);
    memcpy((LPVOID)dwAddress, patchData, dwSize);
    VirtualProtect((LPVOID)dwAddress, dwSize, flOldProtect, &flOldProtect);
};

void SetJmpDest(LPVOID patch, DWORD jmpOffset, LPVOID jmpDest, LPDWORD jmpStorage) {
    *jmpStorage = (DWORD)jmpDest;
    *(DWORD*)((BYTE*)patch + jmpOffset) = (DWORD)jmpStorage;
};

// 0x5DE7E0
BYTE InitShaders_Patch[] = {
    //0x51,                                 // ??: push ecx
    0xFF, 0x25, 0xCD, 0xCD, 0xCD, 0xCD,     // 00: jmp [...]
    //0x59,                                 // ??: pop ecx
    //0xC3,                                 // ??: retn
};

/*
init_shader_thing_1      .text 005DD620 0000008A 00000018 00000010 R . . . . . .
init_shader_thing_2      .text 005DD6B0 0000008A 00000018 00000010 R . . . . . .
init_shader_thing_3      .text 005DD740 0000008A 00000018 00000010 R . . . . . .
init_shader_thing_4      .text 005DD7D0 0000008A 00000018 00000010 R . . . . . .
init_shader_thing_5      .text 005DD860 0000008A 00000018 00000010 R . . . . . .
init_shader_thing_6      .text 005DD8F0 0000008A 00000018 00000010 R . . . . . .
init_shader_thing_7      .text 005DD980 0000008A 00000018 00000010 R . . . . . .
init_shader_thing_8      .text 005DDA10 0000008A 00000018 00000010 R . . . . . .
init_shader_thing_9      .text 005DDAA0 00000087 00000018 00000010 R . . . . . .
init_shader_thing_10     .text 005DDB30 00000087 00000018 00000010 R . . . . . .
init_global_shader_thing .text 005DE730 000000A4 0000001C 00000008 R . . . . . .
*/

FnHook<DWORD,DWORD,DWORD,DWORD,DWORD> lpfnInitShaderThing1;
FnHook<DWORD,DWORD,DWORD,DWORD,DWORD> lpfnInitShaderThing2;
FnHook<DWORD,DWORD,DWORD,DWORD,DWORD> lpfnInitShaderThing3;
FnHook<DWORD,DWORD,DWORD,DWORD,DWORD> lpfnInitShaderThing4;
FnHook<DWORD,DWORD,DWORD,DWORD,DWORD> lpfnInitShaderThing5;
FnHook<DWORD,DWORD,DWORD,DWORD,DWORD> lpfnInitShaderThing6;
FnHook<DWORD,DWORD,DWORD,DWORD,DWORD> lpfnInitShaderThing7;
FnHook<DWORD,DWORD,DWORD,DWORD,DWORD> lpfnInitShaderThing8;
FnHook<DWORD,DWORD,DWORD,DWORD,DWORD> lpfnInitShaderThing9;
FnHook<DWORD,DWORD,DWORD,DWORD,DWORD> lpfnInitShaderThing10;
FnHook<DWORD,DWORD,DWORD>             lpfnInitGlobalShaderThing;

DWORD lpInitShaders_Hook;

DWORD gpuShaders[47];


DWORD InitShader(const LPVOID &This, DWORD type, DWORD index, DWORD offset, DWORD size)
{
    DWORD result = 0;

    if (type == 0)
    {
        result = lpfnInitGlobalShaderThing(This, offset, size);
        LogFile::Format(" - Initialized global shader (offset: 0x%08X, size: 0x%08X), Result = 0x%08X\n",
            offset, size, result);
    }
    else
    {
        DWORD unk = *(DWORD*)((BYTE*)(*(DWORD*)This) + 0xC);

        switch (type)
        {
            case 1:
                result = lpfnInitShaderThing1(This, offset, size, index, unk);
                break;
            case 2:
                result = lpfnInitShaderThing2(This, offset, size, index, unk);
                break;
            case 3:
                result = lpfnInitShaderThing3(This, offset, size, index, unk);
                break;
            case 4:
                result = lpfnInitShaderThing4(This, offset, size, index, unk);
                break;
            case 5:
                result = lpfnInitShaderThing5(This, offset, size, index, unk);
                break;
            case 6:
                result = lpfnInitShaderThing6(This, offset, size, index, unk);
                break;
            case 7:
                result = lpfnInitShaderThing7(This, offset, size, index, unk);
                break;
            case 8:
                result = lpfnInitShaderThing8(This, offset, size, index, unk);
                break;
            case 9:
                result = lpfnInitShaderThing9(This, offset, size, index, unk);
                break;
            case 10:
                result = lpfnInitShaderThing10(This, offset, size, index, unk);
                break;
        }
        LogFile::Format(" - Initialized shader (type: %d, offset: 0x%08X, size: 0x%08X, index: %d, unk: 0x%08X), Result = 0x%08X\n",
            type, offset, size, index, unk, result);
    }
    return result;
}

void InitShaders_Hook() {
    const LPVOID &This = (LPVOID)(*(DWORD*)0x8E9418);

    LogFile::Format("InitShaders_Hook -> %08X\n", This);

    int gameVersion = pDriv3r->GetGameVersion();
    bool useNewSystem = true;

    if (useNewSystem)
    {
        /* Loads shaders from AllShaders.chunk */
        CFileChunkerHook shadersChunk = CFileChunkerHook();

        LogFile::WriteLine("Loading shaders chunk...");
        if (shadersChunk.LoadChunks("shaders\\AllShaders.chunk"))
        {
            int nShaders = shadersChunk.GetChunkCount();

            LogFile::Format(" - Count: %d\n", nShaders);
            for (int i = 0; i < nShaders; i++)
            {
                if (shadersChunk.GetChunkContext(i) == 0x53555047)
                {
                    DWORD bufferSize = shadersChunk.GetChunkSize(i);

                    BYTE *buffer = new BYTE[bufferSize];

                    //LogFile::Format(" - [%d] - Getting data (0x%08X, 0x%08X)\n", i, buffer, bufferSize);
                    shadersChunk.GetChunkData(i, buffer, CHUNKCOPYTYPE_LOAD);

                    while (shadersChunk.IsBusy());

                    int index = *(DWORD*)buffer;
                    int type = shadersChunk.GetChunkVersion(i);

                    DWORD offset = (DWORD)(buffer + 4);
                    DWORD size = bufferSize - 4;

                    //LogFile::Format(" - [%d] - (index: %d, type: %d, offset: %08X, size: %08X)\n", i, index, type, offset, size);

                    gpuShaders[i] = offset;

                    if (InitShader(This, type, index, offset, size) > 0)
                    {
                        shadersChunk.Release();
                        return;
                    }
                }
            }

            shadersChunk.Release();
        }
        else
        {
            MessageBox(pDriv3r->GetMainWindowHwnd(), "FATAL ERROR: Cannot open shaders file!", "D3Hook", MB_OK | MB_ICONERROR);
            ExitProcess(EXIT_FAILURE);
        }
    }
    else
    {
        /* Loads shaders from EXE */
        for (auto shader : shader_programs) {
            auto shaderInfo = shader.info[gameVersion];
            
            if (InitShader(This, shader.type, shader.index, shaderInfo.offset, shaderInfo.size) > 0)
                return;
        }
    }

    __asm {
        mov esi, This
        mov ecx, 0x0B40
        xor eax, eax
        mov edi, 0x8E6718 // EDI is already in use, no need to push/pop it
        rep stosd
    }

    *(DWORD*)0x8E6844 = 1;
    *(DWORD*)0x8E6858 = 2;
    *(DWORD*)0x8E6894 = 3;
    *(DWORD*)0x8E685C = 5;
    *(DWORD*)0x8E6898 = 6;
    *(DWORD*)0x8E68A4 = 9;
    *(DWORD*)0x8E7D2C = 24;
    *(DWORD*)0x8E80EC = 28;
    *(DWORD*)0x8E70C8 = 11;
    *(DWORD*)0x8E7294 = 12;
    *(DWORD*)0x8E72A8 = 12;
    *(DWORD*)0x8E72E4 = 12;
    *(DWORD*)0x8E7F0C = 26;
    *(DWORD*)0x8E8374 = 30;
    *(DWORD*)0x8E83D8 = 32;
    *(DWORD*)0x8E839C = 31;
    *(DWORD*)0x8E8734 = 34;
    *(DWORD*)0x8E8738 = 35;
    *(DWORD*)0x8E874C = 36;
    *(DWORD*)0x8E8788 = 37;

    LogFile::WriteLine("Finished initializing shaders!");
};

void InstallShadersHook(int gameVersion) {
    switch (gameVersion) {
        case __DRIV3R_V100:
        {
            lpfnInitShaderThing1        = 0x5DD620;
            lpfnInitShaderThing2        = 0x5DD6B0;
            lpfnInitShaderThing3        = 0x5DD740;
            lpfnInitShaderThing4        = 0x5DD7D0;
            lpfnInitShaderThing5        = 0x5DD860;
            lpfnInitShaderThing6        = 0x5DD8F0;
            lpfnInitShaderThing7        = 0x5DD980;
            lpfnInitShaderThing8        = 0x5DDA10;
            lpfnInitShaderThing9        = 0x5DDAA0;
            lpfnInitShaderThing10       = 0x5DDB30;
            lpfnInitGlobalShaderThing   = 0x5DE730;

            SetJmpDest(&InitShaders_Patch, 2, &InitShaders_Hook, &lpInitShaders_Hook);
            InstallPatch(0x5DE7E0, InitShaders_Patch, sizeof(InitShaders_Patch));

            LOG("Installed shaders patch!");
        } break;
        case __DRIV3R_V120:
        {
            lpfnInitShaderThing1        = &ReturnNullOrZero;
            lpfnInitShaderThing2        = &ReturnNullOrZero;
            lpfnInitShaderThing3        = &ReturnNullOrZero;
            lpfnInitShaderThing4        = &ReturnNullOrZero;
            lpfnInitShaderThing5        = &ReturnNullOrZero;
            lpfnInitShaderThing6        = &ReturnNullOrZero;
            lpfnInitShaderThing7        = &ReturnNullOrZero;
            lpfnInitShaderThing8        = &ReturnNullOrZero;
            lpfnInitShaderThing9        = &ReturnNullOrZero;
            lpfnInitShaderThing10       = &ReturnNullOrZero;
            lpfnInitGlobalShaderThing   = &ReturnNullOrZero;

            LOG("Shaders patch not installed!");
        } break;
    };
};

// 0x5EB19A - 0x5EB1E9
BYTE ModelLoadLods_Patch[] = {
    0x50,                               // 00: push eax
    0x53,                               // 01: push ebx
    0x56,                               // 02: push esi
    0xFF, 0x15, 0xCD, 0xCD, 0xCD, 0xCD, // 03: call [...]
    0x83, 0xC4, 0x0C,                   // 09: add esp, 0Ch
    0x5E,                               // 12: pop esi
    0x5B,                               // 13: pop ebx
    0x83, 0xC4, 0x0C,                   // 14: add esp, 0Ch
    0xC3                                // 17: retn
};

DWORD lpModelLoadLods_Hook;
FnHook<void,MeshGroup*,DWORD> lpfnModelLoadLod;

void ModelLoadLods_Hook(DWORD lpModel, DWORD lpUnk, ModelContainer *lpContainer) {
    DWORD lod;

    switch (*(WORD*)((BYTE*)lpUnk + 0x62) & 0x1F) {
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

    LodEntry &entry = lpContainer->Entries[lod];

    auto pModels = entry.MeshGroups;
    auto nModels = entry.nMeshGroups;

    if (pModels == NULL)
        return;

    for (int i = 0; i < nModels; i++)
        lpfnModelLoadLod(&pModels[i], lpModel);
};

void InstallPatches(int gameVersion) {
    switch (gameVersion)
    {
        case __DRIV3R_V100:
        {
            InstallShadersHook(gameVersion);

            // BUGFIX: "Taxi taillight bug"
            lpfnModelLoadLod = 0x5EB0A0;

            SetJmpDest(&ModelLoadLods_Patch, 5, &ModelLoadLods_Hook, &lpModelLoadLods_Hook);
            InstallPatch(0x5EB19A, ModelLoadLods_Patch, sizeof(ModelLoadLods_Patch));
        } break;
        case __DRIV3R_V120:
        {
            
        } break;
    }
};

DWORD lpInitialise;
DWORD lpUninitialise;

void NAKED HookInitialize() {
    SetupHooks();

    __asm jmp dword ptr ds : lpInitialise;
}

void NAKED HookUninitialize() {
    __asm jmp dword ptr ds : lpUninitialise;
}

void HookFramework(int gameVersion) {
    DWORD vtFrameworkPtr = 0;

    switch (gameVersion)
    {
        case __DRIV3R_V100:
        {
            vtFrameworkPtr = 0x6F7480;
        } break;
        case __DRIV3R_V120:
        {
            vtFrameworkPtr = 0x788D1C;
        } break;
    }

    // hook into the framework
    InstallVTHook(vtFrameworkPtr + 4, (DWORD)&HookInitialize, &lpInitialise);
    InstallVTHook(vtFrameworkPtr + 8, (DWORD)&HookUninitialize, &lpUninitialise);
    LogFile::WriteLine("Successfully hooked into the framework!");
};

//
// Initialize all the important stuff prior to Driv3r starting up
//
void Initialize(int gameVersion) {
    // first hook into the framework
    HookFramework(gameVersion);

    InstallPatches(gameVersion);

    // now install all of our non-class based hooks
    HookSystem::Initialize(gameVersion);
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            LogFile::WriteLine("D3Hook initialized.");

            HMODULE hDI8Module = NULL;
            RSDSEntry gameInfo;

            LogFile::WriteLine("Checking for known Driv3r versions...");
            if (CDriv3r::GetGameInfo(gameInfo))
            {
                LogFile::Format(" - Detected game version V%1.2f\n", gameInfo.fancy_version);

                if (gameInfo.isSupported)
                {
                    if (LoadSystemLibrary("dinput8.dll", &hDI8Module) &&
                        GetHookProcAddress(hDI8Module, "DirectInput8Create", (FARPROC*)&lpDI8Create))
                    {
                        // initialize game manager
                        pDriv3r = new CDriv3r(gameInfo.version);

                        LogFile::WriteLine("Successfully injected into Driv3r!");
                        Initialize(gameInfo.version);
                    }
                    else
                    {
                        LogFile::WriteLine("Failed to inject into Driv3r!");
                    }
                }
                else
                {
                    LogFile::WriteLine("Unsupported game version! Terminating...");
                    MessageBox(NULL, "Sorry, this version of Driv3r is unsupported. Please remove D3Hook to launch the game.", "D3Hook", MB_OK | MB_ICONERROR);

                    ExitProcess(EXIT_FAILURE);
                }
            }
            else
            {
                LogFile::WriteLine("Unknown module detected! Terminating...");
                MessageBox(NULL, "Unknown module! D3Hook will now terminate the process.", "D3Hook", MB_OK | MB_ICONERROR);

                ExitProcess(EXIT_FAILURE);
            }
        } break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return true;
}