#pragma once

#include "Core.h"
#include "Stellar/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Stellar {

    class ST_API RenderTarget
    {
    public:
        virtual ~RenderTarget() = default;

        virtual void Begin(const glm::vec3& clearColor) = 0;
        virtual void End() = 0;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual const Ref<Texture2D>& GetColorTexture() const = 0;

        static Ref<RenderTarget> Create(uint32_t width, uint32_t height);
    };

}
