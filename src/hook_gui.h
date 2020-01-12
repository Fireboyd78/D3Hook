#pragma once

#include <windows.h>

struct IDirect3DDevice9;
struct ImDrawData;

namespace gui
{
    namespace device
    {
        bool        Initialize(IDirect3DDevice9 *device);
        void        Shutdown();
        void        NewFrame();
        void        RenderDrawData(ImDrawData* draw_data);

        bool        CreateDeviceObjects();
        void        InvalidateDeviceObjects();
    };
    namespace runtime
    {
        bool        Initialize(void *hwnd);
        void        Shutdown();
        void        NewFrame();
        
        LRESULT     WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };

    /*
        Core
    */
    void Reset();

    void BeginFrame();
    void EndFrame();

    void Render();

    /*
        user-defined
    */
    void Initialize(HWND hwnd, IDirect3DDevice9 *device);
    void Shutdown();
    void Update();
};