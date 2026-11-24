#pragma once

#include "Core.h"
#include "Stellar/KeyCodes.h"
#include "Stellar/Renderer/Mesh.h"
#include "Stellar/Renderer/Texture.h"
#include "Stellar/Renderer/TextureAtlas.h"
#include "Stellar/Renderer/SpriteAnimation.h"
#include "Stellar/Renderer/OrthographicCamera.h"
#include "Stellar/Scene/Tilemap.h"
#include "Stellar/Scene/Entity.h"

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Stellar {

    struct TagComponent
    {
        std::string Tag;
    };

    struct TransformComponent
    {
        glm::vec2 Position = { 0.0f, 0.0f };
        float Rotation = 0.0f; // degrees
        glm::vec2 Scale = { 1.0f, 1.0f };

        glm::mat4 GetTransform() const
        {
            return glm::translate(glm::mat4(1.0f), glm::vec3(Position, 0.0f))
                * glm::rotate(glm::mat4(1.0f), glm::radians(Rotation), glm::vec3(0.0f, 0.0f, 1.0f))
                * glm::scale(glm::mat4(1.0f), glm::vec3(Scale, 1.0f));
        }
    };

    struct SpriteRendererComponent
    {
        glm::vec4 Color = glm::vec4(1.0f);
        Ref<Texture2D> Texture;   // optional; sampled bindlessly when set
        Ref<Stellar::Mesh> Mesh;  // optional; defaults to the scene's unit quad

        // Remaps the mesh's texcoords; set from a TextureAtlas via SetSubTexture.
        glm::vec2 UVOffset = { 0.0f, 0.0f };
        glm::vec2 UVScale = { 1.0f, 1.0f };

        // Mirrors the sprite by flipping its texcoords at render time.
        bool FlipX = false;
        bool FlipY = false;

        // Points this sprite at a sub-texture of an atlas.
        void SetSubTexture(const TextureAtlas& atlas, uint32_t id)
        {
            if (atlas.GetTexture())
                Texture = atlas.GetTexture();
            UVOffset = atlas.GetUVOffset(id);
            UVScale = atlas.GetUVScale(id);
        }
    };

    // Plays a SpriteAnimation onto the entity's SpriteRendererComponent;
    // advanced by Scene::OnUpdate.
    struct SpriteAnimatorComponent
    {
        SpriteAnimation Animation;
        float Time = 0.0f;
        float Speed = 1.0f;
        bool Playing = true;

        void Play(const SpriteAnimation& animation)
        {
            Animation = animation;
            Time = 0.0f;
            Playing = true;
        }

        bool IsFinished() const
        {
            return !Animation.Loop && Animation.IsValid() && Time >= Animation.GetDuration();
        }

        // Atlas sub-texture id of the current frame.
        uint32_t GetCurrentFrame() const
        {
            ST_ASSERT(Animation.IsValid(), "Animator has no valid animation!");
            size_t frame = static_cast<size_t>(Time / Animation.FrameTime);
            if (Animation.Loop)
                frame %= Animation.Frames.size();
            else
                frame = frame < Animation.Frames.size() ? frame : Animation.Frames.size() - 1;
            return Animation.Frames[frame];
        }
    };

    // Optional axis-aligned tilemap collision for a moving entity.
    struct TilemapColliderComponent
    {
        Ref<Tilemap> Map;
        float TileSize = 1.0f;
        glm::vec2 MapCenter = { 0.0f, 0.0f };
        glm::vec2 HalfSize = { 0.5f, 0.5f };
    };

    // Top-down character controller processed by Scene::OnUpdate: moves the
    // entity's transform with the bound keys and, when the entity also has a
    // SpriteAnimator/SpriteRenderer, drives directional walk/idle animations.
    struct TopDownMovementComponent
    {
        float MoveSpeed = 1.5f;

        KeyCode UpKey = KeyCode::W;
        KeyCode DownKey = KeyCode::S;
        KeyCode LeftKey = KeyCode::A;
        KeyCode RightKey = KeyCode::D;

        // Side animations: set SideFacesRight to the direction the art is
        // authored in; the sprite is mirrored (FlipX) for the other side.
        // Unset animations are simply skipped.
        bool SideFacesRight = true;
        SpriteAnimation WalkUp, WalkDown, WalkSide;
        SpriteAnimation IdleUp, IdleDown, IdleSide;

        // Runtime state managed by the scene.
        enum class Direction { Down = 0, Up, Side };
        Direction Facing = Direction::Down;
        bool Moving = false;
        bool FacingRight = false;
    };

    struct OrthographicCameraComponent {
        std::shared_ptr<OrthographicCamera> Camera;

        bool Primary = true; // there should be exactly one primary camera per scene
        bool FixedAspectRatio = false;

        float Zoom = 1.0f;
    };

    struct CameraFollowComponent {
        Entity Target;
        glm::vec2 Offset = { 0.0f, 0.0f };

        float SmoothSpeed = 5.0f;
        bool Smooth = false;
    };

    struct CameraZoomComponent {
        float MinZoom = 0.25f;
        float MaxZoom = 8.0f;

        float ZoomSensitivity = 0.85f;
    };
}
