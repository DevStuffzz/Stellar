#include "Platform/OpenGL/OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Stellar {

    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        : m_WindowHandle(windowHandle)
    {
        ST_ASSERT(windowHandle, "Window handle is null!");
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
        ST_ASSERT(status, "Failed to initialize glad!");

        std::printf("OpenGL Info:\n  Vendor:   %s\n  Renderer: %s\n  Version:  %s\n",
            reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
            reinterpret_cast<const char*>(glGetString(GL_RENDERER)),
            reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }

}
