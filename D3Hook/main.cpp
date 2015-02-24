#include "main.h"

CDriv3r                 *pDriv3r = 0;

IDirect3D9              *pD3D;
IDirect3DDevice9        *pD3DDevice;

IDirect3D9Hook          *pD3DHook;
IDirect3DDevice9Hook    *pD3DDeviceHook;

IVTableHook             *pInputHook;
IVTableHook             *pWindowHook;

WNDPROC hProcOld;
LRESULT APIENTRY WndProcNew(HWND, UINT, WPARAM, LPARAM);

bool SubclassGameWindow()
{
    HWND gameWnd = pDriv3r->GetMainWindowHwnd();

    LOG("Subclassing window...");

    if (gameWnd != NULL)
    {
        hProcOld = (WNDPROC)GetWindowLong(gameWnd, GWL_WNDPROC);

        if (hProcOld != NULL && SetWindowLong(gameWnd, GWL_WNDPROC, (LONG)WndProcNew))
        {
            LOG("Successfully subclassed window!");
        }
        else
        {
            LOG("FAILED to subclass window!");
        }

        return true;
    }

    return false;
}

bool HandleKeyPress(DWORD vKey)
{
    switch (vKey) {
        case VK_OEM_3:
            LOG("Hello from D3Hook!");
            return true;
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

#pragma region Broken message handlers
/*
bool HandleMouseMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);

    float *wndX = (float*)(*ptr_8AC278 + 0x14);
    float *wndY = (float*)(*ptr_8AC278 + 0x18);

    if (uMsg == WM_MOVE)
    {
        if (*(bool*)0x8AFA44)
        {
            *wndX = *(float*)x;
            *wndY = *(float*)y;

            __asm {
                push wndX
                push wndY

                mov ecx, __DWPTR_DS(0x8AC238)
                mov esi, [ecx]

                call __DWPTR(esi + 84h);
            }

            char buf[20]; // 'window_size %d %d'

            _snprintf(buf, sizeof(buf), (char*)(0x78C4C0), x, y);

            OutputDebugStringA(buf);

            __asm {
                lea eax, buf
                push eax

                mov ecx, __DWPTR_DS(0x8AC268)
                mov edx, [ecx]

                call __DWPTR(edx + 10h)
            }
        }

        return true;
    }
    else if (uMsg >= WM_MOUSEMOVE)
    {
        int w1 = (wParam >> 3) & 1;
        int w2 = (wParam >> 2) & 1;

        float cX = x / *wndX;
        float cY = y / *wndY;

        // handle mouse events
        switch (uMsg) {
            case WM_MOUSEMOVE:
            {
                int *curX = (int*)(*ptr_8AC278 + 0xC);
                int *curY = (int*)(*ptr_8AC278 + 0x10);

                if (x != *curX || y != *curY)
                {
                    __asm {
                        push cX
                        push cY
                        push w1
                        push w2

                        mov ecx, __DWPTR_DS(0x8AC270)
                        mov esi, [ecx]

                        call __DWPTR(esi + 44h)

                        mov [curX], ebx
                        mov [curY], edi
                    }
                }

                return true;
            }
            case WM_MOUSEWHEEL:
            {
                ScreenToClient(hWnd, (POINT*)lParam);

                __asm {
                    push cX
                    push cY
                    push w1
                    push w2

                    mov ecx, __DWPTR_DS(0x8AC720)
                    mov esi, [ecx]

                    call __DWPTR(esi + 48h);
                }

                return true;
            }
            // Handle click events
            case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDOWN: case WM_RBUTTONUP: case WM_RBUTTONDBLCLK:
            case WM_MBUTTONDOWN: case WM_MBUTTONUP: case WM_MBUTTONDBLCLK:
            {
                __asm {
                    push cX
                    push cY
                    push w1
                    push w2

                    mov ecx, __DWPTR_DS(0x8AC720)
                    mov esi, [ecx]

                    // ==== CALL VALUES ===============
                    // LBUTTONDOWN:    20h < 0  ; 0x201
                    // LBUTTONUP:      24h < 1  ; 0x202
                    // LBUTTONDBLCLK:  28h < 2  ; 0x203
                    // 
                    // RBUTTONDOWN:    38h < 3  ; 0x204
                    // RBUTTONUP:      3Ch < 4  ; 0x205
                    // RBUTTONDBLCLK:  40h < 5  ; 0x206
                    //
                    // MBUTTONDOWN:    2Ch < 6  ; 0x207
                    // MBUTTONUP:      30h < 7  ; 0x208
                    // MBUTTONDBLCLK:  34h < 8  ; 0x209
                    // ================================

                    mov edx, uMsg
                    lea edx, [edx - WM_LBUTTONDOWN] // edx now in 0-8 range

                    mov eax, 20h
                    lea eax, [eax + (edx * 4)] // range is 20h-34h

                    call __DWPTR(esi + eax)
                }

                return true;
            }
        }
    }

    // not handled
    return false;
}

LRESULT APIENTRY WndProcNew(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_KEYUP:
        {
            if (HandleKeyPress((DWORD)wParam))
                return 0;

        } break;

        case WM_DESTROY:
        {
            __asm {
                mov ecx, __DWPTR_DS(0x8AC238)
                mov esi, [ecx]

                call __DWPTR(esi + 80h)
            }

            return 0;
        }
        
        case WM_SETREDRAW:
            return 0;

        case WM_SETCURSOR:
        {
            if ((HWND)wParam == hWnd)
            {
                if (!(*(bool*)(*ptr_8AC278 + 0x8)) && (LOWORD(lParam) == HTCLIENT))
                {
                    SetCursor(0);
                    return 1;
                }
            }
        } goto HANDLE_MSG;

        default:
        {
            // try handling mouse messages
            if (HandleMouseMessage(hWnd, uMsg, wParam, lParam))
                return 0;
        } goto HANDLE_MSG;
        
        HANDLE_MSG:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    // if HANDLE_MSG didn't get it, then Driv3r will handle it
    return CallWindowProc(hProcOld, hWnd, uMsg, wParam, lParam);
}
*/
#pragma endregion

NAKED void __fastcall HOOK_ThisCall(DWORD dwAddress, DWORD dwOffset)
{
    __asm {
        mov ecx, __DWPTR_DS(ecx)
        mov esi, [ecx]

        call __DWPTR(esi + edx)
        retn
    }
}

NAKED LPVOID __fastcall HOOK_GetProcAddress(LPVOID lpVt, DWORD dwOffset)
{
    __asm {
        mov eax, [eax]
        add eax, edx

        retn
    }
}

LRESULT APIENTRY WndProcNew(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    const DWORD *ptr = _PTR(0x8AC278);

    switch (uMsg) {
        case WM_KEYUP:
        {
            if (HandleKeyPress((DWORD)wParam))
                return 0;

        } break;

        case WM_DESTROY:
        {
            //__asm {
            //    mov ecx, __DWPTR_DS(0x8AC238)
            //    mov esi, [ecx]
            //
            //    call __DWPTR(esi + 80h)
            //}

            HOOK_ThisCall(0x8AC238, 0x80);

            return 0;
        }

        case WM_MOVE:
        {
            if (*(bool*)0x8AFA44)
            {
                //int xPos = GET_X_LPARAM(lParam);
                //int yPos = GET_Y_LPARAM(lParam);

                POINTS wndPos = MAKEPOINTS(lParam);
                
                float *wndX = (float*)(*ptr + 0x14);
                float *wndY = (float*)(*ptr + 0x18);

                *wndX = (float)wndPos.x;
                *wndY = (float)wndPos.y;

                //__asm {
                //    fild xPos
                //    fstp [wndX]
                //    push [wndX]
                //
                //    fild yPos
                //    fstp [wndY]
                //    push [wndY]
                //
                //    //mov ecx, __DWPTR_DS(0x8AC238)
                //    //mov esi, [ecx]
                //    //
                //    //call __DWPTR(esi + 84h);
                //}

                __asm {
                    push [wndY]
                    push [wndX]
                }

                HOOK_ThisCall(0x8AC238, 0x84);

                char buf[255]; // 'window_size %d %d'

                _snprintf(buf, sizeof(buf), (char*)(0x78C4C0), xPos, yPos);
                OutputDebugStringA(buf);

                __asm {
                    lea eax, buf
                    push eax
                
                    //mov ecx, __DWPTR_DS(0x8AC268)
                    //mov edx, [ecx]
                    //
                    //// send it off to the message pump!
                    //call __DWPTR(edx + 10h)
                }

                HOOK_ThisCall(0x8AC268, 0x10);
            }

            return 0;
        }

        case WM_SETREDRAW:
            return 0;

        case WM_SETCURSOR:
        {
            if ((HWND)wParam == hWnd)
            {
                if (!(*(bool*)(*ptr + 0x8)) && (LOWORD(lParam) == HTCLIENT))
                {
                    SetCursor(0);
                    return 1;
                }
            }
        } goto HANDLE_MSG;

        case WM_MOUSEWHEEL:
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            ScreenToClient(hWnd, (POINT*)lParam);

            float *wndX = (float*)(*ptr + 0x14);
            float *wndY = (float*)(*ptr + 0x18);

            float x, y;

            int w1 = (wParam >> 2) & 1;
            int w2 = (wParam >> 3) & 1;

            __asm {
                fild xPos
                fdiv [wndX]
                fstp x

                fild yPos
                fdiv [wndY]
                fstp y

                push y
                push x
                push w2
                push w1

                //mov ecx, __DWPTR_DS(0x8AC270)
                //mov esi, [ecx]
                //
                //call __DWPTR(esi + 48h);
            }

            HOOK_ThisCall(0x8AC270, 0x48);

            return 0;
        }

        // Handle click events
        case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN: case WM_RBUTTONUP: case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN: case WM_MBUTTONUP: case WM_MBUTTONDBLCLK:
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            float wndX = *(float*)(*ptr + 0x14);
            float wndY = *(float*)(*ptr + 0x18);

            //float x, y;

            POINTFLOAT pos {
                (xPos / wndX),
                (yPos / wndY)
            };

            int w1 = (wParam >> 2) & 1;
            int w2 = (wParam >> 3) & 1;

            /*
            __asm {
                fild xPos
                fdiv [wndX]
                fstp x

                fild yPos
                fdiv [wndY]
                fstp y

                push x
                push y
                push w1
                push w2

                mov ecx, __DWPTR_DS(0x8AC270)
                mov esi, [ecx]

                // ==== CALL VALUES ===============
                // LBUTTONDOWN:    20h < 0  ; 0x201
                // LBUTTONUP:      24h < 1  ; 0x202
                // LBUTTONDBLCLK:  28h < 2  ; 0x203
                // 
                // RBUTTONDOWN:    38h < 3  ; 0x204
                // RBUTTONUP:      3Ch < 4  ; 0x205
                // RBUTTONDBLCLK:  40h < 5  ; 0x206
                //
                // MBUTTONDOWN:    2Ch < 6  ; 0x207
                // MBUTTONUP:      30h < 7  ; 0x208
                // MBUTTONDBLCLK:  34h < 8  ; 0x209
                // ================================

                mov edx, uMsg
                lea edx, [edx - WM_LBUTTONDOWN] // edx now in 0-8 range
                
                mov eax, 20h
                lea eax, [eax + (edx * 4)] // range is 20h-34h
                
                call __DWPTR(esi + eax)  
            }
            */

            LPVOID proc = pInputHook->GetMemberAddress(0x20 + ((uMsg - WM_LBUTTONDOWN) * 4));

            ((void(__thiscall *)(THIS_ void*, int, int, POINTFLOAT))proc)(pInputHook, w1, w2, pos);
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            int *curX = (int*)(*ptr + 0x0C);
            int *curY = (int*)(*ptr + 0x10);

            if (xPos != *curX || yPos != *curX)
            {
                float *wndX = (float*)(*ptr + 0x14);
                float *wndY = (float*)(*ptr + 0x18);

                float x, y;

                int w1 = (wParam >> 2) & 1;
                int w2 = (wParam >> 3) & 1;

                __asm {
                    fild xPos
                    fdiv [wndX]
                    fstp x

                    fild yPos
                    fdiv [wndY]
                    fstp y

                    push y
                    push x
                    push w2
                    push w1

                    mov ecx, __DWPTR_DS(0x8AC270)
                    mov eax, [ecx]

                    call __DWPTR(eax + 44h)

                    mov curX, ebx
                    mov curY, edi
                }

                return 0;
            }
        } goto HANDLE_MSG;

        HANDLE_MSG:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return CallWindowProc(hProcOld, hWnd, uMsg, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        pDriv3r = new CDriv3r();

        if (SUCCEEDED(SubclassGameWindow()))
        {
            //pD3D = (IDirect3D9*)pDriv3r->GetD3D();
            pD3DDevice = (IDirect3DDevice9*)pDriv3r->GetD3DDevice();

            if (pD3DDevice != NULL)
                LOG("Successfully retrieved the D3DDevice pointer!");

            //pD3DHook = new IDirect3D9Hook;
            //pD3DDeviceHook = new IDirect3DDevice9Hook;

            //pDriv3r->SetD3D((DWORD)pD3DHook);
            //LOG("pDriv3r->SetD3DDevice");
            //pDriv3r->SetD3DDevice(pD3DDeviceHook);

            pInputHook = new IVTableHook(0x8AC270);
        }
    }

	return true;
}