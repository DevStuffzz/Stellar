#include "Stellar/Scene/Scene.h"

#include "Stellar/Scene/Components.h"
#include "Stellar/Scene/Entity.h"
#include "Stellar/Input.h"
#include "Stellar/Renderer/Renderer.h"

#include <algorithm>
#include <cmath>

namespace Stellar {

    Entity Scene::CreateEntity(const std::string& name)
    {
        Entity entity(m_Registry.Create(), this);
        entity.AddComponent<TagComponent>(name);
        entity.AddComponent<TransformComponent>();
        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        if (auto* camera = m_Registry.TryGet<OrthographicCameraComponent>(entity.GetID()))
        {
            if (camera->Primary && camera->Camera == m_MainCamera)
                m_MainCamera.reset();
        }
        m_Registry.Destroy(entity.GetID());
    }

    void Scene::OnUpdate(Timestep ts)
    {
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

        m_Registry.Each<TransformComponent, SpriteRendererComponent>(
            [this](EntityID, TransformComponent& transform, SpriteRendererComponent& sprite)
            {
                const Ref<Mesh>& mesh = sprite.Mesh ? sprite.Mesh : m_QuadMesh;
                if (sprite.Texture)
                {
                    glm::vec2 uvOffset = sprite.UVOffset;
                    glm::vec2 uvScale = sprite.UVScale;
                    if (sprite.FlipX) { uvOffset.x += uvScale.x; uvScale.x = -uvScale.x; }
                    if (sprite.FlipY) { uvOffset.y += uvScale.y; uvScale.y = -uvScale.y; }
                    Renderer::Submit(mesh, sprite.Texture, transform.GetTransform(), sprite.Color, uvOffset, uvScale);
                }
                else
                    Renderer::Submit(mesh, transform.GetTransform(), sprite.Color);
            });

        Renderer::EndScene();
    }

}
