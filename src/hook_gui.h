#pragma once

#include <windows.h>
#include "imgui.h"

struct IDirect3DDevice9;
//struct ImDrawData;
//struct ImGuiIO;

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

    //#########################################################################
    // 
    // Implementation interface for a GUI menu handler
    // 
    // ------------------------------------------------------------------------
    // 
    // Requires the following method(s) to be implemented:
    // 
    // * void ShowGUI(bool *p_open);
    // * - Called when the GUI system has requested this menu to appear
    // * - Menu state is controlled via the bool pointer parameter 'p_open'
    // 
    // ========================================================================
    // Remarks:
    // 
    // * Uses CRTP to implement base methods without virtual functions:
    // * - https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
    // 
    //#########################################################################
    struct IMenuHandler
    {
        //
        // Derived classes MUST provide an implementation of this method!
        //
        void ShowGUI(bool *p_open);
    };

    namespace menus
    {
        typedef bool(*update_t)(void);

        namespace fx
        {
            class menu_base {
            protected:
                menu_base *next;

                void Register();
            public:
                menu_base(menu_base &) = delete;
                menu_base(const menu_base &&) = delete;

                menu_base() {
                    Register();
                }

                virtual ~menu_base();

                virtual void Update() {
                    /* do nothing */
                }

                static void UpdateAll();
                static void FreeAll();
            };

            bool AlwaysTrue();
        };

        template <class TClass>
        class menu_handler : public fx::menu_base
        {
            TClass *owner;
            update_t check_fn;
            bool show_menu;

        public:
            explicit menu_handler(IMenuHandler *_owner, update_t _check_fn)
                : owner(static_cast<TClass *>(_owner)), check_fn(_check_fn), show_menu(false), menu_base() {}

            bool IsAvailable() {
                return check_fn();
            }

            bool IsActive() {
                return show_menu;
            }

            //void Toggle() {
            //    if (check_fn())
            //        show_menu = !show_menu;
            //}

            template <typename... TArgs>
            void Push(bool (*toggle_fn)(const char *, TArgs...), const char *label, TArgs... args)
            {
                bool disabled = !check_fn();

                ImGui::BeginDisabled(disabled);

                if (toggle_fn(label, args...))
                    show_menu = !show_menu;

                ImGui::EndDisabled();
            }

            void PushButton(const char *label)
            {
                bool disabled = !check_fn();

                ImGui::BeginDisabled(disabled);

                if (ImGui::Button(label))
                    show_menu = !show_menu;

                ImGui::EndDisabled();
            }

            void Update() override {
                if (show_menu)
                    owner->ShowGUI(&show_menu);
            }
        };

        void UpdateAll();
        void FreeAll();

        template <class TClass>
        const auto Create(TClass *obj, update_t check_fn = fx::AlwaysTrue)
        {
            static_assert(std::is_base_of<IMenuHandler, TClass>::value,
                "**** Handlers MUST publicly inherit from 'gui::IMenuHandler' and implement a 'ShowGUI(bool *p_open)' method! ****");

            return new menu_handler<TClass>(obj, check_fn);
        }
    };

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