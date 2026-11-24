#pragma once

#include "Core.h"
#include "Stellar/Renderer/Mesh.h"
#include "Stellar/Renderer/OrthographicCamera.h"
#include "Stellar/Renderer/RenderCommand.h"
#include "Stellar/Renderer/Shader.h"
#include "Stellar/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Stellar {

    class ST_API Renderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void OnWindowResize(uint32_t width, uint32_t height);

        static void BeginScene(const OrthographicCamera& camera);
        static void EndScene();

        // Draws with the built-in 2D shader; color tints the mesh's vertex colors.
        static void Submit(const Ref<Mesh>& mesh, const glm::mat4& transform = glm::mat4(1.0f), const glm::vec4& color = glm::vec4(1.0f));

        // Draws with the built-in 2D shader sampling the texture through its bindless handle.
        // uvOffset/uvScale remap the mesh's texcoords (e.g. onto a TextureAtlas sub-texture).
        static void Submit(const Ref<Mesh>& mesh, const Ref<Texture2D>& texture, const glm::mat4& transform = glm::mat4(1.0f), const glm::vec4& tint = glm::vec4(1.0f), const glm::vec2& uvOffset = glm::vec2(0.0f), const glm::vec2& uvScale = glm::vec2(1.0f));

        // Draws with a custom shader. Sets u_ViewProjection and u_Transform.
        static void Submit(const Ref<Shader>& shader, const Ref<Mesh>& mesh, const glm::mat4& transform = glm::mat4(1.0f));

        static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    };

}
