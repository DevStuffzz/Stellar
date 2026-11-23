#include "Stellar/Input.h"

namespace Stellar {

    static Input& GetInstance()
    {
        static Scope<Input> s_Instance = Input::Create();
        return *s_Instance;
    }

    bool Input::IsKeyPressed(KeyCode key)
    {
        return GetInstance().IsKeyPressedImpl(key);
    }

    bool Input::IsMouseButtonPressed(MouseButton button)
    {
        return GetInstance().IsMouseButtonPressedImpl(button);
    }

    glm::vec2 Input::GetMousePosition()
    {
        return GetInstance().GetMousePositionImpl();
    }

    float Input::GetMouseX()
    {
        return GetMousePosition().x;
    }

    float Input::GetMouseY()
    {
        return GetMousePosition().y;
    }

}
