#include "Platform/OpenGL/OpenGLRenderTarget.h"

#include <stdexcept>

namespace Stellar {

    OpenGLRenderTarget::OpenGLRenderTarget(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height)
    {
        GLint maxTextureSize = 0;
        GLint maxViewport[2] = {};
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        glGetIntegerv(GL_MAX_VIEWPORT_DIMS, maxViewport);
        if (width == 0 || height == 0 || width > static_cast<uint32_t>(maxTextureSize) ||
            height > static_cast<uint32_t>(maxTextureSize) || width > static_cast<uint32_t>(maxViewport[0]) ||
            height > static_cast<uint32_t>(maxViewport[1]))
            throw std::invalid_argument("Render target dimensions are invalid or exceed OpenGL limits");

        m_ColorTexture = Texture2D::Create(width, height);
        glCreateFramebuffers(1, &m_Framebuffer);
        glNamedFramebufferTexture(m_Framebuffer, GL_COLOR_ATTACHMENT0, m_ColorTexture->GetRendererID(), 0);
        if (glCheckNamedFramebufferStatus(m_Framebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            glDeleteFramebuffers(1, &m_Framebuffer);
            m_Framebuffer = 0;
            throw std::runtime_error("OpenGL render target framebuffer is incomplete");
        }
    }

    OpenGLRenderTarget::~OpenGLRenderTarget()
    {
        if (m_Bound)
            End();
        if (m_Framebuffer)
            glDeleteFramebuffers(1, &m_Framebuffer);
    }

    void OpenGLRenderTarget::Begin(const glm::vec3& clearColor)
    {
        if (m_Bound)
            throw std::logic_error("Render target is already bound");

        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_PreviousFramebuffer);
        glGetIntegerv(GL_VIEWPORT, m_PreviousViewport);
        glGetIntegerv(GL_BLEND_SRC_RGB, &m_PreviousBlendSrcRGB);
        glGetIntegerv(GL_BLEND_DST_RGB, &m_PreviousBlendDstRGB);
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &m_PreviousBlendSrcAlpha);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &m_PreviousBlendDstAlpha);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffer);
        glViewport(0, 0, static_cast<GLsizei>(m_Width), static_cast<GLsizei>(m_Height));
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        const GLfloat color[] = { clearColor.r, clearColor.g, clearColor.b, 1.0f };
        glClearBufferfv(GL_COLOR, 0, color);
        m_Bound = true;
    }

    void OpenGLRenderTarget::End()
    {
        if (!m_Bound)
            throw std::logic_error("Render target is not bound");

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<GLuint>(m_PreviousFramebuffer));
        glViewport(m_PreviousViewport[0], m_PreviousViewport[1], m_PreviousViewport[2], m_PreviousViewport[3]);
        glBlendFuncSeparate(m_PreviousBlendSrcRGB, m_PreviousBlendDstRGB,
            m_PreviousBlendSrcAlpha, m_PreviousBlendDstAlpha);
        m_Bound = false;
    }

}
