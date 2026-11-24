#include "SandboxApp.h"
#include <algorithm>
#include <cmath>
#include <random>

#include <vector>

    namespace {
        constexpr float tileSize = 0.2f;
    }


    SandboxApp::SandboxApp()
        : Stellar::Application(Stellar::WindowProps("Stellar Sandbox", 1280, 720))
    {
        m_Scene = Stellar::CreateRef<Stellar::Scene>();
        CreateSceneEntities();
        m_Scene->OnViewportResize(GetWindow().GetWidth(), GetWindow().GetHeight());
    }

    SandboxApp::~SandboxApp()
    {
    }


    void SandboxApp::CreateSceneEntities()
    {
        Stellar::Ref<Stellar::Texture2D> blocks = Stellar::Texture2D::Create("assets/textures/atlas/blocks.png");
        m_BlocksAtlas = Stellar::CreateRef<Stellar::TextureAtlas>(blocks, 16, 16);

        const uint32_t mapWidth = 256;
        const uint32_t mapHeight = 256;

        int grass = 0;
        int stone = 1;
        int path = 2;
        int sand = 16 * 11;
        int water = 12 * 16 + 14;

        m_Tilemap = Stellar::CreateRef<Stellar::Tilemap>(
            mapWidth, mapHeight
        );

        m_Tilemap->SetTileCollision(water, true);
        m_Tilemap->SetTileCollision(stone, true);
        m_Tilemap->Clear(grass);

        std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist(0.0f, 10000.0f);

        glm::vec2 offset(dist(rng), dist(rng));

        float scale = 0.045f;

        for (int y = 0; y < mapHeight; y++)
        {
            for (int x = 0; x < mapWidth; x++)
            {
                glm::vec2 pos = glm::vec2(x, y) * scale + offset;

                float elevation =
                    glm::perlin(pos) * 0.65f +
                    glm::perlin(pos * 2.0f) * 0.25f +
                    glm::perlin(pos * 4.0f) * 0.10f;

                uint16_t tile = grass;

                if (elevation < -0.20f)
                    tile = water;
                else if (elevation < -0.12f)
                    tile = sand;
                else if (elevation > 0.28f)
                    tile = stone;

                m_Tilemap->SetTile(x, y, tile);
            }
        }

        // Keep the player's spawn and its immediate surroundings walkable.
        for (uint32_t y = mapHeight / 2 - 2; y < mapHeight / 2 + 2; y++)
            for (uint32_t x = mapWidth / 2 - 2; x < mapWidth / 2 + 2; x++)
                m_Tilemap->SetTile(x, y, grass);

        m_TilemapEntity = m_Scene->CreateEntity("Tilemap");

    // Player: created last so it draws on top; animated from the characters atlas.
    Stellar::Ref<Stellar::Texture2D> characters = Stellar::Texture2D::Create("assets/textures/atlas/characters.png");
    // characters.png uses 18x20 px cells; each character is a 12-column x 3-row
    // block: row 0 = up/back, row 1 = side (faces left), row 2 = down/front.
    Stellar::Ref<Stellar::TextureAtlas> charactersAtlas = Stellar::CreateRef<Stellar::TextureAtlas>(characters, 18, 20);

    const uint32_t atlasColumns = charactersAtlas->GetColumns();
    const uint32_t blockRow = 3, blockColumn = 0; // blonde villager block

    auto frame = [&](uint32_t row, uint32_t column) {
        return (blockRow + row) * atlasColumns + blockColumn + column;
    };
    auto walk = [&](uint32_t row) {
        // Walk cycle is columns 0-2; ping-pong through the middle stand pose.
        return Stellar::SpriteAnimation(charactersAtlas,
            std::vector<uint32_t>{ frame(row, 0), frame(row, 1), frame(row, 2), frame(row, 1) }, 0.15f);
    };
    auto idle = [&](uint32_t row) {
        // Static standing pose.
        return Stellar::SpriteAnimation(charactersAtlas, std::vector<uint32_t>{ frame(row, 1) });
    };

    m_Player = m_Scene->CreateEntity("Player");
    m_Player.GetComponent<Stellar::TransformComponent>().Scale = { 0.225f, 0.25f }; // 18:20 aspect
    m_Player.AddComponent<Stellar::SpriteRendererComponent>();
    m_Player.AddComponent<Stellar::SpriteAnimatorComponent>();

    auto& collider = m_Player.AddComponent<Stellar::TilemapColliderComponent>();
    collider.Map = m_Tilemap;
    collider.TileSize = tileSize;
    collider.HalfSize = { 0.075f, 0.06f };

    auto& movement = m_Player.AddComponent<Stellar::TopDownMovementComponent>();
    movement.MoveSpeed = 1.5f;
    movement.WalkUp = walk(0);
    movement.WalkSide = walk(1);
    movement.WalkDown = walk(2);
    movement.IdleUp = idle(0);
    movement.IdleSide = idle(1);
    movement.IdleDown = idle(2);

    m_CameraEntity = m_Scene->CreateEntity("Camera");
    auto& camera = m_CameraEntity.AddComponent<Stellar::OrthographicCameraComponent>();
    camera.Camera = Stellar::CreateRef<Stellar::OrthographicCamera>(-1.0f, 1.0f, -1.0f, 1.0f);
    camera.Zoom = 1.8f;
    m_CameraEntity.AddComponent<Stellar::CameraFollowComponent>().Target = m_Player;
    auto& follow = m_CameraEntity.GetComponent<Stellar::CameraFollowComponent>();
    follow.Smooth = true;
    m_CameraEntity.AddComponent<Stellar::CameraZoomComponent>();
    
    }


    void SandboxApp::OnUpdate(Stellar::Timestep ts)
    {
        if (Stellar::Input::IsKeyPressed(Stellar::KeyCode::Escape))
            Close();

        m_Scene->OnUpdate(ts);
        UpdateTilemapMesh();

        Stellar::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.12f, 1.0f });
        Stellar::RenderCommand::Clear();

        m_Scene->OnRender();
    }

    void SandboxApp::OnWindowResize(uint32_t width, uint32_t height)
    {
        m_Scene->OnViewportResize(width, height);
    }

    void SandboxApp::OnMouseScroll(float xOffset, float yOffset)
    {
        m_Scene->OnMouseScroll(xOffset, yOffset);
    }

    void SandboxApp::UpdateTilemapMesh()
    {
        const auto& transform = m_TilemapEntity.GetComponent<Stellar::TransformComponent>();
        const auto bounds = m_Tilemap->GetVisibleBounds(m_Scene->GetPrimaryCamera(), tileSize, transform.GetTransform());
        if (m_HasVisibleTileBounds && bounds == m_VisibleTileBounds)
            return;

        auto mesh = m_Tilemap->GenerateMesh(*m_BlocksAtlas, bounds, tileSize);
        if (mesh)
        {
            if (!m_TilemapEntity.HasComponent<Stellar::SpriteRendererComponent>())
                m_TilemapEntity.AddComponent<Stellar::SpriteRendererComponent>();
            auto& sprite = m_TilemapEntity.GetComponent<Stellar::SpriteRendererComponent>();
            sprite.Mesh = mesh;
            sprite.Texture = m_BlocksAtlas->GetTexture();
        }
        else if (m_TilemapEntity.HasComponent<Stellar::SpriteRendererComponent>())
            m_TilemapEntity.RemoveComponent<Stellar::SpriteRendererComponent>();

        m_VisibleTileBounds = bounds;
        m_HasVisibleTileBounds = true;
    }

    Stellar::Application* Stellar::CreateApplication()
    {
        return new SandboxApp();
    }


