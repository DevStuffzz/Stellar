#include "Platform/Windows/WindowsWindow.h"

#include "Stellar/Renderer/RendererAPI.h"

#include <GLFW/glfw3.h>

namespace Stellar {

    static uint32_t s_GLFWWindowCount = 0;

    static void GLFWErrorCallback(int error, const char* description)
    {
        std::fprintf(stderr, "[GLFW] Error (%d): %s\n", error, description);
    }

    Scope<Window> Window::Create(const WindowProps& props)
    {
        return CreateScope<WindowsWindow>(props);
    }

    WindowsWindow::WindowsWindow(const WindowProps& props)
    {
        Init(props);
    }

    WindowsWindow::~WindowsWindow()
    {
        Shutdown();
    }

    void WindowsWindow::Init(const WindowProps& props)
    {
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;

        if (s_GLFWWindowCount == 0)
        {
            glfwSetErrorCallback(GLFWErrorCallback);
            int success = glfwInit();
            ST_ASSERT(success, "Could not initialize GLFW!");
        }

        if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
        {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        #ifdef ST_DEBUG
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        #endif
        }

        m_Window = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height), m_Data.Title.c_str(), nullptr, nullptr);
        ST_ASSERT(m_Window, "Could not create GLFW window!");
        s_GLFWWindowCount++;

        m_Context = GraphicsContext::Create(m_Window);
        m_Context->Init();

        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(props.VSync);

        // Use the framebuffer size so the viewport stays correct on high-DPI displays.
        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
        m_Data.Width = static_cast<uint32_t>(fbWidth);
        m_Data.Height = static_cast<uint32_t>(fbHeight);

        glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
        {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            data.Width = static_cast<uint32_t>(width);
            data.Height = static_cast<uint32_t>(height);

            if (data.ResizeCallback && width > 0 && height > 0)
                data.ResizeCallback(data.Width, data.Height);
        });
    }

    void WindowsWindow::Shutdown()
    {
        m_Context.reset();
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;

        if (--s_GLFWWindowCount == 0)
            glfwTerminate();
    }

    void WindowsWindow::OnUpdate()
    {
        m_Context->SwapBuffers();
        glfwPollEvents();
    }

    bool WindowsWindow::ShouldClose() const
    {
        return glfwWindowShouldClose(m_Window);
    }

    void WindowsWindow::SetVSync(bool enabled)
    {
        glfwSwapInterval(enabled ? 1 : 0);
        m_Data.VSync = enabled;
    }

    double WindowsWindow::GetTime() const
    {
        return glfwGetTime();
    }

}
