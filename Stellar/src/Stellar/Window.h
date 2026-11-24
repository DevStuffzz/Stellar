#pragma once

#include "Core.h"

#include <cstdint>
#include <functional>
#include <string>

namespace Stellar {

    struct WindowProps
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;
        bool VSync;

        WindowProps(const std::string& title = "Stellar Engine", uint32_t width = 1280, uint32_t height = 720, bool vsync = true)
            : Title(title), Width(width), Height(height), VSync(vsync) {}
    };

    // Platform-agnostic window interface. Implemented per platform (e.g. Platform/Windows/WindowsWindow).
    class ST_API Window
    {
    public:
        using ResizeCallbackFn = std::function<void(uint32_t width, uint32_t height)>;
        using ScrollCallbackFn = std::function<void(float xOffset, float yOffset)>;

        virtual ~Window() = default;

        // Polls OS events and presents the back buffer.
        virtual void OnUpdate() = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        virtual bool ShouldClose() const = 0;

        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual void SetResizeCallback(const ResizeCallbackFn& callback) = 0;
        virtual void SetScrollCallback(const ScrollCallbackFn& callback) = 0;

        // Seconds since the window system was initialized.
        virtual double GetTime() const = 0;

        virtual void* GetNativeWindow() const = 0;

        static Scope<Window> Create(const WindowProps& props = WindowProps());
    };

}
