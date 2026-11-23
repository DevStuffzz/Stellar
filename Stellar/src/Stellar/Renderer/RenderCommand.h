#pragma once

#include "Core.h"
#include "Stellar/Renderer/RendererAPI.h"

namespace Stellar {

    // Thin static wrapper around the active RendererAPI.
    class ST_API RenderCommand
    {
    public:
        static void Init();
        static void Shutdown();

        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
        static void SetClearColor(const glm::vec4& color);
        static void Clear();
        static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0);
    };

}
