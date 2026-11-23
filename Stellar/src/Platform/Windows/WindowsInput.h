#pragma once

#include "Stellar/Input.h"

namespace Stellar {

    class WindowsInput : public Input
    {
    protected:
        bool IsKeyPressedImpl(KeyCode key) const override;
        bool IsMouseButtonPressedImpl(MouseButton button) const override;
        glm::vec2 GetMousePositionImpl() const override;
    };

}
