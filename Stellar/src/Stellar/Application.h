#pragma once

#include "Core.h"
#include "Stellar/Window.h"
#include "Stellar/Timestep.h"

namespace Stellar {

    class ST_API Application
    {
    public:
        Application(const WindowProps& props = WindowProps());
        virtual ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        void Run();
        void Close();

        Window& GetWindow() { return *m_Window; }

        static Application& Get();

    protected:
        virtual void OnUpdate(Timestep ts) {}
        virtual void OnWindowResize(uint32_t width, uint32_t height) {}
        virtual void OnMouseScroll(float xOffset, float yOffset) {}

    private:
        Scope<Window> m_Window;
        bool m_Running = true;
        double m_LastFrameTime = 0.0;

        static Application* s_Instance;
    };

    // Defined by the client application.
    Application* CreateApplication();

}
