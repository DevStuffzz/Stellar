#include "SandboxApp.h"

#include <vector>

using namespace Stellar;

SandboxApp::SandboxApp()
    : Application(WindowProps("Stellar Sandbox", 1280, 720)), m_Camera(-1.0f, 1.0f, -1.0f, 1.0f)
{
    UpdateCameraProjection(GetWindow().GetWidth(), GetWindow().GetHeight());

    m_Scene = CreateRef<Scene>();
    CreateSceneEntities();
}

SandboxApp::~SandboxApp()
{
}

void SandboxApp::CreateSceneEntities()
{

    Ref<Texture2D> blocks = Texture2D::Create("assets/textures/atlas/blocks.png");
    // blocks.png is a 16x16 grid of 16px tiles.
    Ref<TextureAtlas> blocksAtlas = CreateRef<TextureAtlas>(blocks->GetWidth(), blocks->GetHeight(), 16, 16);

    // Full atlas texture for reference.
    {
        Entity quad = m_Scene->CreateEntity("Atlas");

        auto& transform = quad.GetComponent<TransformComponent>();
        transform.Position = { -0.8f, -0.4f };
        transform.Scale = { 0.4f, 0.4f };

        quad.AddComponent<SpriteRendererComponent>().Texture = blocks;
    }

    // First row of sub-textures, addressed by id (0 = top-left, row-major).
    for (uint32_t id = 0; id < 8; id++)
    {
        Entity tile = m_Scene->CreateEntity("SubTexture");

        auto& transform = tile.GetComponent<TransformComponent>();
        transform.Position = { -0.7f + id * 0.2f, 0.4f };
        transform.Scale = { 0.19f, 0.19f };

        auto& sprite = tile.AddComponent<SpriteRendererComponent>();
        sprite.Texture = blocks;
        sprite.UVOffset = blocksAtlas->GetUVOffset(id);
        sprite.UVScale = blocksAtlas->GetUVScale(id);
    }

    // Player: created last so it draws on top; uses a single sub-texture.
    m_Player = m_Scene->CreateEntity("Player");
    m_Player.GetComponent<TransformComponent>().Scale = { 0.25f, 0.25f };
    auto& playerSprite = m_Player.AddComponent<SpriteRendererComponent>();
    playerSprite.Texture = blocks;
    playerSprite.SetSubTexture(*blocksAtlas, 0);
}

void SandboxApp::OnUpdate(Timestep ts)
{
    if (Input::IsKeyPressed(KeyCode::Escape))
        Close();

    // Player movement and rotation.
    auto& playerTransform = m_Player.GetComponent<TransformComponent>();

    const float moveSpeed = 1.5f * ts;
    if (Input::IsKeyPressed(KeyCode::A)) playerTransform.Position.x -= moveSpeed;
    if (Input::IsKeyPressed(KeyCode::D)) playerTransform.Position.x += moveSpeed;
    if (Input::IsKeyPressed(KeyCode::W)) playerTransform.Position.y += moveSpeed;
    if (Input::IsKeyPressed(KeyCode::S)) playerTransform.Position.y -= moveSpeed;

    const float rotateSpeed = 180.0f * ts;
    if (Input::IsKeyPressed(KeyCode::Q)) playerTransform.Rotation += rotateSpeed;
    if (Input::IsKeyPressed(KeyCode::E)) playerTransform.Rotation -= rotateSpeed;

    // Hold the left mouse button to tint the player.
    m_Player.GetComponent<SpriteRendererComponent>().Color = Input::IsMouseButtonPressed(MouseButton::Left)
        ? glm::vec4(1.0f, 0.4f, 0.2f, 1.0f)
        : glm::vec4(0.2f, 0.6f, 1.0f, 1.0f);

    // Camera movement.
    glm::vec3 cameraPosition = m_Camera.GetPosition();
    const float cameraSpeed = 1.0f * m_ZoomLevel * ts;
    if (Input::IsKeyPressed(KeyCode::Left))  cameraPosition.x -= cameraSpeed;
    if (Input::IsKeyPressed(KeyCode::Right)) cameraPosition.x += cameraSpeed;
    if (Input::IsKeyPressed(KeyCode::Up))    cameraPosition.y += cameraSpeed;
    if (Input::IsKeyPressed(KeyCode::Down))  cameraPosition.y -= cameraSpeed;
    m_Camera.SetPosition(cameraPosition);

    RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.12f, 1.0f });
    RenderCommand::Clear();

    m_Scene->OnRender(m_Camera);
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
