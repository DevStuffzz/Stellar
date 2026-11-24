#include "Stellar/Scene/Scene.h"

#include "Stellar/Scene/Components.h"
#include "Stellar/Scene/Entity.h"
#include "Stellar/Input.h"
#include "Stellar/Renderer/Renderer.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace Stellar {

    const Ref<Mesh>& TilemapRendererComponent::GetMesh(const OrthographicCamera& camera, const glm::mat4& transform)
    {
        if (!Map || !Atlas || !Atlas->GetTexture())
            throw std::invalid_argument("Tilemap renderer requires a map and a textured atlas");
        if (!std::isfinite(TileSize) || TileSize <= 0.0f)
            throw std::invalid_argument("Tilemap renderer tile size must be positive and finite");
        const float determinant = glm::determinant(camera.GetViewProjectionMatrix() * transform);
        if (!std::isfinite(determinant) || determinant == 0.0f)
            throw std::invalid_argument("Tilemap renderer requires an invertible camera and transform");

        const auto bounds = Map->GetVisibleBounds(camera, TileSize, transform);
        bool changed = !m_HasCachedMesh || Map != m_CachedMap ||
            !(bounds == m_CachedBounds) || TileSize != m_CachedTileSize ||
            Map->GetWidth() != m_CachedMapWidth || Map->GetHeight() != m_CachedMapHeight ||
            Atlas->GetTextureWidth() != m_CachedTextureWidth ||
            Atlas->GetTextureHeight() != m_CachedTextureHeight ||
            Atlas->GetCellWidth() != m_CachedCellWidth || Atlas->GetCellHeight() != m_CachedCellHeight;

        size_t index = 0;
        for (uint32_t y = bounds.MinY; y < bounds.MaxY; ++y)
        {
            for (uint32_t x = bounds.MinX; x < bounds.MaxX; ++x)
            {
                const int tile = Map->GetTile(x, y);
                if (tile < Tilemap::EmptyTile ||
                    (tile != Tilemap::EmptyTile && static_cast<uint32_t>(tile) >= Atlas->GetSubTextureCount()))
                    throw std::invalid_argument("Tilemap contains a tile outside the atlas");
                if (!changed && tile != m_CachedTiles[index])
                    changed = true;
                ++index;
            }
        }

        if (changed)
        {
            std::vector<int> tiles;
            tiles.reserve(index);
            for (uint32_t y = bounds.MinY; y < bounds.MaxY; ++y)
                for (uint32_t x = bounds.MinX; x < bounds.MaxX; ++x)
                    tiles.push_back(Map->GetTile(x, y));

            Ref<Mesh> mesh = Map->GenerateMesh(*Atlas, bounds, TileSize);
            m_Mesh = std::move(mesh);
            m_CachedMap = Map;
            m_CachedBounds = bounds;
            m_CachedTiles = std::move(tiles);
            m_CachedTileSize = TileSize;
            m_CachedMapWidth = Map->GetWidth();
            m_CachedMapHeight = Map->GetHeight();
            m_CachedTextureWidth = Atlas->GetTextureWidth();
            m_CachedTextureHeight = Atlas->GetTextureHeight();
            m_CachedCellWidth = Atlas->GetCellWidth();
            m_CachedCellHeight = Atlas->GetCellHeight();
            m_HasCachedMesh = true;
        }
        return m_Mesh;
    }

    namespace {
        struct CallbackScope
        {
            unsigned int& Depth;
            explicit CallbackScope(unsigned int& depth) : Depth(depth) { ++Depth; }
            ~CallbackScope() { --Depth; }
        };
    }

    Scene::~Scene()
    {
        std::vector<EntityID> scripts;
        m_Registry.Each<NativeScriptComponent>(
            [&scripts](EntityID id, NativeScriptComponent&) { scripts.push_back(id); });
        for (EntityID id : scripts)
            RemoveNativeScript(id);
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        Entity entity(m_Registry.Create(), this);
        entity.AddComponent<TagComponent>(name);
        entity.AddComponent<TransformComponent>();
        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        if (entity.GetScene() != this || !entity)
            throw std::invalid_argument("Cannot destroy an invalid entity or an entity from another scene");
        m_ScriptOperations.push_back({ entity.GetID(), true });
        ProcessScriptOperations();
    }

    void Scene::RemoveNativeScript(EntityID entity)
    {
        m_ScriptOperations.push_back({ entity, false });
        ProcessScriptOperations();
    }

    void Scene::ProcessScriptOperations()
    {
        if (m_ScriptCallbackDepth || m_ProcessingScriptOperations)
            return;

        m_ProcessingScriptOperations = true;
        struct ProcessingScope
        {
            bool& Processing;
            ~ProcessingScope() { Processing = false; }
        } processingScope{ m_ProcessingScriptOperations };

        while (!m_ScriptOperations.empty())
        {
            const ScriptOperation operation = m_ScriptOperations.front();
            m_ScriptOperations.pop_front();
            if (!m_Registry.Valid(operation.Entity))
                continue;

            if (auto* script = m_Registry.TryGet<NativeScriptComponent>(operation.Entity))
            {
                if (script->Instance)
                {
                    CallbackScope callback(m_ScriptCallbackDepth);
                    script->Instance->OnDestroy();
                }
                m_Registry.Remove<NativeScriptComponent>(operation.Entity);
            }

            if (operation.DestroyEntity)
            {
                if (auto* camera = m_Registry.TryGet<OrthographicCameraComponent>(operation.Entity))
                {
                    if (camera->Primary && camera->Camera == m_MainCamera)
                        m_MainCamera.reset();
                }
                m_Registry.Destroy(operation.Entity);
            }
        }
    }

    void Scene::OnUpdate(Timestep ts)
    {
        // Snapshot IDs so callbacks can create or remove scripts safely.
        std::vector<EntityID> scripts;
        m_Registry.Each<NativeScriptComponent>(
            [&scripts](EntityID id, NativeScriptComponent&) { scripts.push_back(id); });
        for (EntityID id : scripts)
        {
            auto* script = m_Registry.TryGet<NativeScriptComponent>(id);
            if (!script)
                continue;

            if (!script->Instance)
            {
                if (!script->InstantiateScript)
                    throw std::logic_error("NativeScriptComponent must be bound before updating");

                script->Instance = script->InstantiateScript();
                if (!script->Instance)
                    throw std::runtime_error("Native script factory returned null");
                script->Instance->m_Entity = Entity(id, this);
                {
                    CallbackScope callback(m_ScriptCallbackDepth);
                    script->Instance->OnCreate();
                }
                ProcessScriptOperations();
                script = m_Registry.TryGet<NativeScriptComponent>(id);
                if (!script || !script->Instance)
                    continue;
            }

            {
                CallbackScope callback(m_ScriptCallbackDepth);
                script->Instance->OnUpdate(ts);
            }
            ProcessScriptOperations();
        }

        // Top-down movement controllers.
        m_Registry.Each<TopDownMovementComponent, TransformComponent>(
            [this, ts](EntityID entity, TopDownMovementComponent& movement, TransformComponent& transform)
            {
                glm::vec2 move = { 0.0f, 0.0f };
                if (Input::IsKeyPressed(movement.LeftKey))  move.x -= 1.0f;
                if (Input::IsKeyPressed(movement.RightKey)) move.x += 1.0f;
                if (Input::IsKeyPressed(movement.UpKey))    move.y += 1.0f;
                if (Input::IsKeyPressed(movement.DownKey))  move.y -= 1.0f;

                using Direction = TopDownMovementComponent::Direction;
                const bool moving = move.x != 0.0f || move.y != 0.0f;
                Direction facing = movement.Facing;
                bool facingRight = movement.FacingRight;

                if (moving)
                {
                    const glm::vec2 displacement = glm::normalize(move) * movement.MoveSpeed * static_cast<float>(ts);
                    if (auto* collider = m_Registry.TryGet<TilemapColliderComponent>(entity))
                    {
                        ST_ASSERT(collider->Map, "Tilemap collider requires a map!");
                        transform.Position = collider->Map->MoveWithCollisions(transform.Position, collider->HalfSize,
                            displacement, collider->TileSize, collider->MapCenter);
                    }
                    else
                        transform.Position += displacement;

                    // The dominant axis picks the facing.
                    if (glm::abs(move.x) >= glm::abs(move.y))
                    {
                        facing = Direction::Side;
                        facingRight = move.x > 0.0f;
                    }
                    else
                    {
                        facing = move.y > 0.0f ? Direction::Up : Direction::Down;
                    }
                }

                if (auto* sprite = m_Registry.TryGet<SpriteRendererComponent>(entity))
                    sprite->FlipX = facing == Direction::Side && facingRight != movement.SideFacesRight;

                if (auto* animator = m_Registry.TryGet<SpriteAnimatorComponent>(entity))
                {
                    const SpriteAnimation* animation = nullptr;
                    switch (facing)
                    {
                        case Direction::Up:   animation = moving ? &movement.WalkUp : &movement.IdleUp; break;
                        case Direction::Side: animation = moving ? &movement.WalkSide : &movement.IdleSide; break;
                        default:              animation = moving ? &movement.WalkDown : &movement.IdleDown; break;
                    }

                    const bool stateChanged = facing != movement.Facing || moving != movement.Moving;
                    if (animation->IsValid() && (stateChanged || !animator->Animation.IsValid()))
                        animator->Play(*animation);
                }

                movement.Facing = facing;
                movement.Moving = moving;
                movement.FacingRight = facingRight;
            });

        // Sprite animators.
        m_Registry.Each<SpriteAnimatorComponent, SpriteRendererComponent>(
            [ts](EntityID, SpriteAnimatorComponent& animator, SpriteRendererComponent& sprite)
            {
                if (!animator.Animation.IsValid())
                    return;

                if (animator.Playing)
                    animator.Time += ts * animator.Speed;

                sprite.SetSubTexture(*animator.Animation.Atlas, animator.GetCurrentFrame());
            });

        m_MainCamera.reset();
        bool mainCamFound = false;
        m_Registry.Each<OrthographicCameraComponent, TransformComponent>(
            [this, ts, &mainCamFound](EntityID entity, OrthographicCameraComponent& camera,
                TransformComponent& transform)
            {
                ST_ASSERT(camera.Camera, "Camera component requires a camera!");
                if (!camera.Camera)
                    return;

                if (auto* follow = m_Registry.TryGet<CameraFollowComponent>(entity))
                {
                    ST_ASSERT(follow->Target.GetScene() == this && follow->Target,
                        "Camera follow target must be a valid entity in the same scene!");
                    if (follow->Target.GetScene() != this || !follow->Target)
                        return;

                    ST_ASSERT(follow->Target.HasComponent<TransformComponent>(),
                        "Camera follow target requires a transform!");
                    if (!follow->Target.HasComponent<TransformComponent>())
                        return;

                    const glm::vec2 target = follow->Target.GetComponent<TransformComponent>().Position
                        + follow->Offset;
                    if (follow->Smooth)
                    {
                        ST_ASSERT(follow->SmoothSpeed >= 0.0f, "Camera smooth speed cannot be negative!");
                        const float alpha = 1.0f - std::exp(-std::max(follow->SmoothSpeed, 0.0f)
                            * static_cast<float>(ts));
                        transform.Position += (target - transform.Position) * alpha;
                    }
                    else
                        transform.Position = target;
                }

                camera.Camera->SetPosition({
                    transform.Position.x,
                    transform.Position.y,
                    0.0f
                    });
                camera.Camera->SetRotation(transform.Rotation);
                UpdateCameraProjection(camera);

                if (camera.Primary) {
                    ST_ASSERT(!mainCamFound, "Scene must have exactly one primary camera!");
                    mainCamFound = true;
                    m_MainCamera = camera.Camera;
                }
            });

    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
            return;

        m_ViewportWidth = width;
        m_ViewportHeight = height;
        m_Registry.Each<OrthographicCameraComponent>(
            [this](EntityID, OrthographicCameraComponent& camera)
            {
                UpdateCameraProjection(camera);
            });
    }

    void Scene::OnMouseScroll(float, float yOffset)
    {
        m_Registry.Each<OrthographicCameraComponent, CameraZoomComponent>(
            [this, yOffset](EntityID, OrthographicCameraComponent& camera, CameraZoomComponent& zoom)
            {
                if (!camera.Primary)
                    return;

                ST_ASSERT(zoom.MinZoom > 0.0f && zoom.MaxZoom >= zoom.MinZoom,
                    "Camera zoom limits must be positive and ordered!");
                ST_ASSERT(zoom.ZoomSensitivity > 0.0f, "Camera zoom sensitivity must be positive!");
                if (zoom.MinZoom <= 0.0f || zoom.MaxZoom < zoom.MinZoom || zoom.ZoomSensitivity <= 0.0f)
                    return;

                camera.Zoom = std::clamp(camera.Zoom * std::pow(zoom.ZoomSensitivity, yOffset),
                    zoom.MinZoom, zoom.MaxZoom);
                UpdateCameraProjection(camera);
            });
    }

    const OrthographicCamera& Scene::GetPrimaryCamera() const
    {
        ST_ASSERT(m_MainCamera, "Scene has no primary camera!");
        return *m_MainCamera;
    }

    void Scene::UpdateCameraProjection(OrthographicCameraComponent& camera)
    {
        ST_ASSERT(camera.Camera, "Camera component requires a camera!");
        ST_ASSERT(camera.Zoom > 0.0f, "Camera zoom must be positive!");
        if (!camera.Camera || camera.Zoom <= 0.0f)
            return;

        const float aspectRatio = camera.FixedAspectRatio
            ? 1.0f
            : static_cast<float>(m_ViewportWidth) / static_cast<float>(m_ViewportHeight);
        camera.Camera->SetProjection(-aspectRatio * camera.Zoom, aspectRatio * camera.Zoom,
            -camera.Zoom, camera.Zoom);
    }

    void Scene::OnRender()
    {
        ST_ASSERT(m_MainCamera, "Scene has no primary camera!");
        if (!m_MainCamera)
            return;

        // Created lazily so a Scene can be constructed before the renderer.
        if (!m_QuadMesh)
            m_QuadMesh = Mesh::CreateQuad();

        Renderer::BeginScene(*m_MainCamera);

        m_Registry.Each<TransformComponent>(
            [this](EntityID entity, TransformComponent& transform)
            {
                const glm::mat4 matrix = transform.GetTransform();
                if (auto* tilemap = m_Registry.TryGet<TilemapRendererComponent>(entity))
                {
                    const Ref<Mesh>& mesh = tilemap->GetMesh(*m_MainCamera, matrix);
                    if (mesh)
                        Renderer::Submit(mesh, tilemap->Atlas->GetTexture(), matrix, tilemap->Color);
                }

                auto* spriteComponent = m_Registry.TryGet<SpriteRendererComponent>(entity);
                if (!spriteComponent)
                    return;
                auto& sprite = *spriteComponent;
                const Ref<Mesh>& mesh = sprite.Mesh ? sprite.Mesh : m_QuadMesh;
                if (sprite.Texture)
                {
                    glm::vec2 uvOffset = sprite.UVOffset;
                    glm::vec2 uvScale = sprite.UVScale;
                    if (sprite.FlipX) { uvOffset.x += uvScale.x; uvScale.x = -uvScale.x; }
                    if (sprite.FlipY) { uvOffset.y += uvScale.y; uvScale.y = -uvScale.y; }
                    Renderer::Submit(mesh, sprite.Texture, matrix, sprite.Color, uvOffset, uvScale);
                }
                else
                    Renderer::Submit(mesh, matrix, sprite.Color);
            });

        Renderer::EndScene();
    }

}
