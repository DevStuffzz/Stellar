#pragma once

#include "Core.h"
#include "Stellar/Renderer/Mesh.h"
#include "Stellar/Renderer/Texture.h"
#include "Stellar/Renderer/TextureAtlas.h"

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

        // Points this sprite at a sub-texture of an atlas.
        void SetSubTexture(const TextureAtlas& atlas, uint32_t id)
        {
            if (atlas.GetTexture())
                Texture = atlas.GetTexture();
            UVOffset = atlas.GetUVOffset(id);
            UVScale = atlas.GetUVScale(id);
        }
    };

}
