#include "Stellar/Application.h"

#include "Stellar/Renderer/Renderer.h"

namespace Stellar {

    Application* Application::s_Instance = nullptr;

    Application::Application(const WindowProps& props)
    {
        ST_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        m_Window = Window::Create(props);
        m_Window->SetResizeCallback([this](uint32_t width, uint32_t height)
        {
            Renderer::OnWindowResize(width, height);
            OnWindowResize(width, height);
        });

        Renderer::Init();
        m_LastFrameTime = m_Window->GetTime();
    }

    Application::~Application()
    {
        Renderer::Shutdown();
        s_Instance = nullptr;
    }

    void Application::Run()
    {
        while (m_Running && !m_Window->ShouldClose())
        {
            double time = m_Window->GetTime();
            Timestep ts = static_cast<float>(time - m_LastFrameTime);
            m_LastFrameTime = time;

            // Skip rendering while minimized, but keep pumping events.
            if (m_Window->GetWidth() > 0 && m_Window->GetHeight() > 0)
                OnUpdate(ts);

            m_Window->OnUpdate();
        }
    }

    void Application::Close()
    {
        m_Running = false;
    }

    Application& Application::Get()
    {
        return *s_Instance;
    }

}
