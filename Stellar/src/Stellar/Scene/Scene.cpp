#include "Stellar/Scene/Scene.h"

#include "Stellar/Scene/Components.h"
#include "Stellar/Scene/Entity.h"
#include "Stellar/Input.h"
#include "Stellar/Renderer/Renderer.h"

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
    }

    void Scene::OnRender(const OrthographicCamera& camera)
    {
        // Created lazily so a Scene can be constructed before the renderer.
        if (!m_QuadMesh)
            m_QuadMesh = Mesh::CreateQuad();

        Renderer::BeginScene(camera);

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
