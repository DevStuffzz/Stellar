#pragma once

#include "Core.h"
#include "Stellar/Renderer/VertexArray.h"

#include <vector>

#include <glm/glm.hpp>

namespace Stellar {

    struct Vertex2D
    {
        glm::vec2 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
    };

    // API-agnostic 2D mesh: owns its GPU vertex/index buffers via a VertexArray.
    class ST_API Mesh
    {
    public:
        Mesh(const std::vector<Vertex2D>& vertices, const std::vector<uint32_t>& indices);

        const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }
        uint32_t GetIndexCount() const { return m_IndexCount; }

        // Unit primitives centered at the origin.
        static Ref<Mesh> CreateQuad(const glm::vec4& color = glm::vec4(1.0f));
        static Ref<Mesh> CreateTriangle(const glm::vec4& color = glm::vec4(1.0f));
        static Ref<Mesh> CreateCircle(uint32_t segments = 32, const glm::vec4& color = glm::vec4(1.0f));

    private:
        Ref<VertexArray> m_VertexArray;
        uint32_t m_IndexCount = 0;
    };

}
