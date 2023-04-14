#pragma once

#include <windows.h>

struct IDirect3DDevice9;
struct ImDrawData;
struct ImGuiIO;

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
    bool IsReady();
    bool IsActive();

    bool Initialize(HWND hwnd, IDirect3DDevice9 *device);
    
    void Shutdown();
    void Reset();

    void Toggle();

    void BeginFrame();
    void EndFrame();

    void Render();

    LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /*
        user-defined
    */
    bool SetupConfig(ImGuiIO &io);

    void OnReady(ImGuiIO &io);
    void OnShutdown();

    void Update();
    void Draw();
};