#pragma once

#include "Stellar/Renderer/RenderTarget.h"

#include <glad/glad.h>

namespace Stellar {

    class OpenGLRenderTarget : public RenderTarget
    {
    public:
        OpenGLRenderTarget(uint32_t width, uint32_t height);
        ~OpenGLRenderTarget() override;

        void Begin(const glm::vec3& clearColor) override;
        void End() override;
        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }
        const Ref<Texture2D>& GetColorTexture() const override { return m_ColorTexture; }

    private:
        uint32_t m_Width, m_Height;
        GLuint m_Framebuffer = 0;
        Ref<Texture2D> m_ColorTexture;
        GLint m_PreviousFramebuffer = 0;
        GLint m_PreviousViewport[4] = {};
        GLint m_PreviousBlendSrcRGB = 0, m_PreviousBlendDstRGB = 0;
        GLint m_PreviousBlendSrcAlpha = 0, m_PreviousBlendDstAlpha = 0;
        bool m_Bound = false;
    };

}
