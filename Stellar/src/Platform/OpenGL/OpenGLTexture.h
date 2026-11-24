#pragma once

#include "Stellar/Renderer/Texture.h"

#include <glad/glad.h>

namespace Stellar {

    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(uint32_t width, uint32_t height);
        OpenGLTexture2D(const std::string& path);
        virtual ~OpenGLTexture2D();

        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }
        uint32_t GetRendererID() const override { return m_RendererID; }
        uint64_t GetHandle() const override;

        void SetData(const void* data, uint32_t size) override;

        bool operator==(const Texture& other) const override
        {
            return m_RendererID == other.GetRendererID();
        }

    private:
        void CreateStorage(uint32_t mipLevels);

    private:
        std::string m_Path;
        uint32_t m_Width = 0, m_Height = 0;
        uint32_t m_MipLevels = 1;
        uint32_t m_RendererID = 0;

        // Lazily created because sampler state is immutable once a bindless
        // handle exists; resident until destruction.
        mutable uint64_t m_Handle = 0;
    };

}
