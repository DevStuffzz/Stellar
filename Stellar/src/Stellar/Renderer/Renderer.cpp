#include "Stellar/Renderer/Renderer.h"

#include <stdexcept>

namespace Stellar {

    struct RendererData
    {
        glm::mat4 ViewProjection = glm::mat4(1.0f);
        Ref<Shader> DefaultShader;
        Ref<Shader> Shader2D;
        Ref<Texture2D> WhiteTexture;
    };

    static RendererData* s_Data = nullptr;

    static const char* s_DefaultVertexSrc = R"(
        #version 450 core

        layout(location = 0) in vec2 a_Position;
        layout(location = 1) in vec4 a_Color;
        layout(location = 2) in vec2 a_TexCoord;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;
        uniform vec2 u_UVOffset;
        uniform vec2 u_UVScale;

        out vec4 v_Color;
        out vec2 v_TexCoord;

        void main()
        {
            v_Color = a_Color;
            v_TexCoord = u_UVOffset + a_TexCoord * u_UVScale;
            gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 0.0, 1.0);
        }
    )";

    static const char* s_DefaultFragmentSrc = R"(
        #version 450 core
        #extension GL_ARB_bindless_texture : require

        layout(location = 0) out vec4 o_Color;

        in vec4 v_Color;
        in vec2 v_TexCoord;

        uniform vec4 u_Color;
        layout(bindless_sampler) uniform sampler2D u_Texture;

        void main()
        {
            o_Color = texture(u_Texture, v_TexCoord) * v_Color * u_Color;
        }
    )";

    void Renderer::Init()
    {
        RenderCommand::Init();

        s_Data = new RendererData();
        s_Data->DefaultShader = Shader::Create("Default2D", s_DefaultVertexSrc, s_DefaultFragmentSrc);
        if (!s_Data->DefaultShader)
            throw std::runtime_error("Failed to create the built-in 2D shader");
        s_Data->Shader2D = s_Data->DefaultShader;

        // 1x1 white texture so untextured draws share the textured shader path.
        s_Data->WhiteTexture = Texture2D::Create(1, 1);
        uint32_t whitePixel = 0xffffffff;
        s_Data->WhiteTexture->SetData(&whitePixel, sizeof(whitePixel));
    }

    void Renderer::Shutdown()
    {
        delete s_Data;
        s_Data = nullptr;

        RenderCommand::Shutdown();
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        RenderCommand::SetViewport(0, 0, width, height);
    }

    void Renderer::BeginScene(const OrthographicCamera& camera)
    {
        s_Data->ViewProjection = camera.GetViewProjectionMatrix();
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Set2DShader(const Ref<Shader>& shader)
    {
        s_Data->Shader2D = shader ? shader : s_Data->DefaultShader;
    }

    void Renderer::Submit(const Ref<Mesh>& mesh, const glm::mat4& transform, const glm::vec4& color)
    {
        Submit(mesh, s_Data->WhiteTexture, transform, color);
    }

    void Renderer::Submit(const Ref<Mesh>& mesh, const Ref<Texture2D>& texture, const glm::mat4& transform, const glm::vec4& tint, const glm::vec2& uvOffset, const glm::vec2& uvScale)
    {
        s_Data->Shader2D->Bind();
        s_Data->Shader2D->SetFloat4("u_Color", tint);
        s_Data->Shader2D->SetTextureHandle("u_Texture", texture->GetHandle());
        s_Data->Shader2D->SetFloat2("u_UVOffset", uvOffset);
        s_Data->Shader2D->SetFloat2("u_UVScale", uvScale);
        Submit(s_Data->Shader2D, mesh, transform);
    }

    void Renderer::Submit(const Ref<Shader>& shader, const Ref<Mesh>& mesh, const glm::mat4& transform)
    {
        shader->Bind();
        shader->SetMat4("u_ViewProjection", s_Data->ViewProjection);
        shader->SetMat4("u_Transform", transform);

        RenderCommand::DrawIndexed(mesh->GetVertexArray(), mesh->GetIndexCount());
    }

}
