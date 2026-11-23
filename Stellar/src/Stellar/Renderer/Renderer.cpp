#include "Stellar/Renderer/Renderer.h"

namespace Stellar {

    struct RendererData
    {
        glm::mat4 ViewProjection = glm::mat4(1.0f);
        Ref<Shader> DefaultShader;
    };

    static RendererData* s_Data = nullptr;

    static const char* s_DefaultVertexSrc = R"(
        #version 450 core

        layout(location = 0) in vec2 a_Position;
        layout(location = 1) in vec4 a_Color;
        layout(location = 2) in vec2 a_TexCoord;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;

        out vec4 v_Color;
        out vec2 v_TexCoord;

        void main()
        {
            v_Color = a_Color;
            v_TexCoord = a_TexCoord;
            gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 0.0, 1.0);
        }
    )";

    static const char* s_DefaultFragmentSrc = R"(
        #version 450 core

        layout(location = 0) out vec4 o_Color;

        in vec4 v_Color;
        in vec2 v_TexCoord;

        uniform vec4 u_Color;

        void main()
        {
            o_Color = v_Color * u_Color;
        }
    )";

    void Renderer::Init()
    {
        RenderCommand::Init();

        s_Data = new RendererData();
        s_Data->DefaultShader = Shader::Create("Default2D", s_DefaultVertexSrc, s_DefaultFragmentSrc);
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

    void Renderer::Submit(const Ref<Mesh>& mesh, const glm::mat4& transform, const glm::vec4& color)
    {
        s_Data->DefaultShader->Bind();
        s_Data->DefaultShader->SetFloat4("u_Color", color);
        Submit(s_Data->DefaultShader, mesh, transform);
    }

    void Renderer::Submit(const Ref<Shader>& shader, const Ref<Mesh>& mesh, const glm::mat4& transform)
    {
        shader->Bind();
        shader->SetMat4("u_ViewProjection", s_Data->ViewProjection);
        shader->SetMat4("u_Transform", transform);

        RenderCommand::DrawIndexed(mesh->GetVertexArray(), mesh->GetIndexCount());
    }

}
