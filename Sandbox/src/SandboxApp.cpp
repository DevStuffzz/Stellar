#include "SandboxApp.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace Stellar;

static glm::mat4 MakeTransform(const glm::vec2& position, float rotationDegrees, const glm::vec2& scale)
{
    return glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
        * glm::rotate(glm::mat4(1.0f), glm::radians(rotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f))
        * glm::scale(glm::mat4(1.0f), glm::vec3(scale, 1.0f));
}

SandboxApp::SandboxApp()
    : Application(WindowProps("Stellar Sandbox", 1280, 720)), m_Camera(-1.0f, 1.0f, -1.0f, 1.0f)
{
    UpdateCameraProjection(GetWindow().GetWidth(), GetWindow().GetHeight());

    m_Quad = Mesh::CreateQuad();
    m_Circle = Mesh::CreateCircle(48);

    // Custom per-vertex colors.
    m_Triangle = CreateRef<Mesh>(
        std::vector<Vertex2D>{
            { { -0.5f, -0.5f }, { 1.0f, 0.2f, 0.2f, 1.0f }, { 0.0f, 0.0f } },
            { {  0.5f, -0.5f }, { 0.2f, 1.0f, 0.2f, 1.0f }, { 1.0f, 0.0f } },
            { {  0.0f,  0.5f }, { 0.2f, 0.2f, 1.0f, 1.0f }, { 0.5f, 1.0f } }
        },
        std::vector<uint32_t>{ 0, 1, 2 });
}

SandboxApp::~SandboxApp()
{
}

void SandboxApp::OnUpdate(Timestep ts)
{
    if (Input::IsKeyPressed(KeyCode::Escape))
        Close();

    const float moveSpeed = 1.5f * ts;
    if (Input::IsKeyPressed(KeyCode::A)) m_PlayerPosition.x -= moveSpeed;
    if (Input::IsKeyPressed(KeyCode::D)) m_PlayerPosition.x += moveSpeed;
    if (Input::IsKeyPressed(KeyCode::W)) m_PlayerPosition.y += moveSpeed;
    if (Input::IsKeyPressed(KeyCode::S)) m_PlayerPosition.y -= moveSpeed;

    const float rotateSpeed = 180.0f * ts;
    if (Input::IsKeyPressed(KeyCode::Q)) m_PlayerRotation += rotateSpeed;
    if (Input::IsKeyPressed(KeyCode::E)) m_PlayerRotation -= rotateSpeed;

    glm::vec3 cameraPosition = m_Camera.GetPosition();
    const float cameraSpeed = 1.0f * m_ZoomLevel * ts;
    if (Input::IsKeyPressed(KeyCode::Left))  cameraPosition.x -= cameraSpeed;
    if (Input::IsKeyPressed(KeyCode::Right)) cameraPosition.x += cameraSpeed;
    if (Input::IsKeyPressed(KeyCode::Up))    cameraPosition.y += cameraSpeed;
    if (Input::IsKeyPressed(KeyCode::Down))  cameraPosition.y -= cameraSpeed;
    m_Camera.SetPosition(cameraPosition);

    // Hold the left mouse button to tint the player.
    glm::vec4 playerColor = Input::IsMouseButtonPressed(MouseButton::Left)
        ? glm::vec4(1.0f, 0.4f, 0.2f, 1.0f)
        : glm::vec4(0.2f, 0.6f, 1.0f, 1.0f);

    RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.12f, 1.0f });
    RenderCommand::Clear();

    Renderer::BeginScene(m_Camera);

    // Background grid of quads.
    for (int y = -5; y <= 5; y++)
    {
        for (int x = -8; x <= 8; x++)
        {
            glm::vec4 color = ((x + y) & 1) ? glm::vec4(0.18f, 0.18f, 0.22f, 1.0f) : glm::vec4(0.14f, 0.14f, 0.17f, 1.0f);
            Renderer::Submit(m_Quad, MakeTransform({ x * 0.2f, y * 0.2f }, 0.0f, { 0.19f, 0.19f }), color);
        }
    }

    Renderer::Submit(m_Triangle, MakeTransform({ -0.8f, 0.4f }, 0.0f, { 0.4f, 0.4f }));
    Renderer::Submit(m_Circle, MakeTransform({ 0.8f, 0.4f }, 0.0f, { 0.35f, 0.35f }), { 0.9f, 0.8f, 0.2f, 1.0f });
    Renderer::Submit(m_Quad, MakeTransform(m_PlayerPosition, m_PlayerRotation, { 0.25f, 0.25f }), playerColor);

    Renderer::EndScene();
}

void SandboxApp::OnWindowResize(uint32_t width, uint32_t height)
{
    UpdateCameraProjection(width, height);
}

void SandboxApp::UpdateCameraProjection(uint32_t width, uint32_t height)
{
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    m_Camera.SetProjection(-aspectRatio * m_ZoomLevel, aspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
}

Stellar::Application* Stellar::CreateApplication()
{
    return new SandboxApp();
}
