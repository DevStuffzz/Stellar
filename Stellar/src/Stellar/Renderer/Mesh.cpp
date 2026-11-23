#include "Stellar/Renderer/Mesh.h"

#include <cmath>

namespace Stellar {

    Mesh::Mesh(const std::vector<Vertex2D>& vertices, const std::vector<uint32_t>& indices)
        : m_IndexCount(static_cast<uint32_t>(indices.size()))
    {
        m_VertexArray = VertexArray::Create();

        Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices.data(), static_cast<uint32_t>(vertices.size() * sizeof(Vertex2D)));
        vertexBuffer->SetLayout({
            { ShaderDataType::Float2, "a_Position" },
            { ShaderDataType::Float4, "a_Color" },
            { ShaderDataType::Float2, "a_TexCoord" }
        });
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        m_VertexArray->SetIndexBuffer(IndexBuffer::Create(indices.data(), m_IndexCount));
    }

    Ref<Mesh> Mesh::CreateQuad(const glm::vec4& color)
    {
        std::vector<Vertex2D> vertices = {
            { { -0.5f, -0.5f }, color, { 0.0f, 0.0f } },
            { {  0.5f, -0.5f }, color, { 1.0f, 0.0f } },
            { {  0.5f,  0.5f }, color, { 1.0f, 1.0f } },
            { { -0.5f,  0.5f }, color, { 0.0f, 1.0f } }
        };
        std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };
        return CreateRef<Mesh>(vertices, indices);
    }

    Ref<Mesh> Mesh::CreateTriangle(const glm::vec4& color)
    {
        std::vector<Vertex2D> vertices = {
            { { -0.5f, -0.5f }, color, { 0.0f, 0.0f } },
            { {  0.5f, -0.5f }, color, { 1.0f, 0.0f } },
            { {  0.0f,  0.5f }, color, { 0.5f, 1.0f } }
        };
        std::vector<uint32_t> indices = { 0, 1, 2 };
        return CreateRef<Mesh>(vertices, indices);
    }

    Ref<Mesh> Mesh::CreateCircle(uint32_t segments, const glm::vec4& color)
    {
        ST_ASSERT(segments >= 3, "A circle needs at least 3 segments!");

        std::vector<Vertex2D> vertices;
        std::vector<uint32_t> indices;
        vertices.reserve(segments + 1);
        indices.reserve(segments * 3);

        vertices.push_back({ { 0.0f, 0.0f }, color, { 0.5f, 0.5f } });
        const float step = 2.0f * 3.14159265358979f / static_cast<float>(segments);
        for (uint32_t i = 0; i < segments; i++)
        {
            float x = std::cos(step * i) * 0.5f;
            float y = std::sin(step * i) * 0.5f;
            vertices.push_back({ { x, y }, color, { x + 0.5f, y + 0.5f } });

            indices.push_back(0);
            indices.push_back(i + 1);
            indices.push_back((i + 1) % segments + 1);
        }

        return CreateRef<Mesh>(vertices, indices);
    }

}
