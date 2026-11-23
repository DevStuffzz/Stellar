#include "Platform/Windows/WindowsInput.h"

#include "Stellar/Application.h"

#include <GLFW/glfw3.h>

namespace Stellar {

    Scope<Input> Input::Create()
    {
        return CreateScope<WindowsInput>();
    }

    static GLFWwindow* GetNativeWindow()
    {
        return static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    }

    bool WindowsInput::IsKeyPressedImpl(KeyCode key) const
    {
        int state = glfwGetKey(GetNativeWindow(), static_cast<int>(key));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool WindowsInput::IsMouseButtonPressedImpl(MouseButton button) const
    {
        return glfwGetMouseButton(GetNativeWindow(), static_cast<int>(button)) == GLFW_PRESS;
    }

    glm::vec2 WindowsInput::GetMousePositionImpl() const
    {
        double x = 0.0, y = 0.0;
        glfwGetCursorPos(GetNativeWindow(), &x, &y);
        return { static_cast<float>(x), static_cast<float>(y) };
    }

}
