#pragma once

#include "Stellar/Window.h"
#include "Stellar/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Stellar {

    class WindowsWindow : public Window
    {
    public:
        WindowsWindow(const WindowProps& props);
        virtual ~WindowsWindow();

        void OnUpdate() override;

        uint32_t GetWidth() const override { return m_Data.Width; }
        uint32_t GetHeight() const override { return m_Data.Height; }

        bool ShouldClose() const override;

        void SetVSync(bool enabled) override;
        bool IsVSync() const override { return m_Data.VSync; }

        void SetResizeCallback(const ResizeCallbackFn& callback) override { m_Data.ResizeCallback = callback; }
        void SetScrollCallback(const ScrollCallbackFn& callback) override { m_Data.ScrollCallback = callback; }

        double GetTime() const override;

        void* GetNativeWindow() const override { return m_Window; }

    private:
        void Init(const WindowProps& props);
        void Shutdown();

    private:
        GLFWwindow* m_Window = nullptr;
        Scope<GraphicsContext> m_Context;

        struct WindowData
        {
            std::string Title;
            uint32_t Width = 0;
            uint32_t Height = 0;
            bool VSync = true;
            ResizeCallbackFn ResizeCallback;
            ScrollCallbackFn ScrollCallback;
        };

        WindowData m_Data;
    };

}
