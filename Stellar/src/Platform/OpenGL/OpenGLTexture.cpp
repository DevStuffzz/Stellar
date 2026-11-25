#include "Platform/OpenGL/OpenGLTexture.h"

#include "Platform/OpenGL/OpenGLBindlessTexture.h"

#include <algorithm>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "Vendor/stb_image.h"

namespace Stellar {

    static uint32_t CalculateMipLevels(uint32_t width, uint32_t height)
    {
        uint32_t levels = 1;
        uint32_t size = std::max(width, height);
        while (size > 1)
        {
            size /= 2;
            levels++;
        }
        return levels;
    }

    void OpenGLTexture2D::CreateStorage(uint32_t mipLevels)
    {
        m_MipLevels = mipLevels;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, mipLevels, GL_RGBA8, m_Width, m_Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER,
            mipLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height)
    {
        CreateStorage(1);
    }

    OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
        : m_Path(path)
    {
        int width = 0, height = 0, channels = 0;
        stbi_set_flip_vertically_on_load(1);
        // Force RGBA so every texture shares one internal format and alignment.
        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
        if (!data)
        {
            std::fprintf(stderr, "[Texture] Failed to load image '%s': %s\n",
                path.c_str(), stbi_failure_reason());
            ST_ASSERT(false, "Failed to load image!");

            // Fall back to a 1x1 magenta texture so the error is visible.
            m_Width = m_Height = 1;
            CreateStorage(1);
            uint32_t magenta = 0xffff00ff;
            glTextureSubImage2D(m_RendererID, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &magenta);
            return;
        }

        m_Width = static_cast<uint32_t>(width);
        m_Height = static_cast<uint32_t>(height);

        CreateStorage(CalculateMipLevels(m_Width, m_Height));
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        if (m_MipLevels > 1)
            glGenerateTextureMipmap(m_RendererID);

        stbi_image_free(data);
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        if (m_Handle && GLBindless::IsSupported())
            GLBindless::MakeTextureHandleNonResidentARB(m_Handle);
        glDeleteTextures(1, &m_RendererID);
    }

    uint64_t OpenGLTexture2D::GetHandle() const
    {
        if (m_Handle == 0)
        {
            ST_ASSERT(GLBindless::IsSupported(), "GL_ARB_bindless_texture is not supported by this driver!");
            if (!GLBindless::IsSupported())
                return 0;

            m_Handle = GLBindless::GetTextureHandleARB(m_RendererID);
            GLBindless::MakeTextureHandleResidentARB(m_Handle);
        }
        return m_Handle;
    }

    void OpenGLTexture2D::SetData(const void* data, uint32_t size)
    {
        ST_ASSERT(size == m_Width * m_Height * 4, "SetData size must match the texture (RGBA8)!");
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        if (m_MipLevels > 1)
            glGenerateTextureMipmap(m_RendererID);
    }

}
