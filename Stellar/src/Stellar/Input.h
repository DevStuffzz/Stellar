#pragma once

#include "Core.h"
#include "Stellar/KeyCodes.h"

#include <glm/glm.hpp>

namespace Stellar {

    // Polling-based input. The static API forwards to a platform implementation
    // (e.g. Platform/Windows/WindowsInput) that queries the application's window.
    class ST_API Input
    {
    public:
        virtual ~Input() = default;

        static bool IsKeyPressed(KeyCode key);
        static bool IsMouseButtonPressed(MouseButton button);
        static glm::vec2 GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();

        static Scope<Input> Create();

    protected:
        virtual bool IsKeyPressedImpl(KeyCode key) const = 0;
        virtual bool IsMouseButtonPressedImpl(MouseButton button) const = 0;
        virtual glm::vec2 GetMousePositionImpl() const = 0;
    };

}
