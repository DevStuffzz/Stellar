#pragma once

#include "Core.h"

#include <cstdint>
#include <string>

namespace Stellar {
    class ST_API Texture
    {
    public:
        virtual ~Texture() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetRendererID() const = 0;

        // Bindless GPU handle (GL_ARB_bindless_texture). Made resident on first
        // use and stays resident for the texture's lifetime, so no texture slots
        // are ever consumed.
        virtual uint64_t GetHandle() const = 0;

        // Uploads raw RGBA8 pixels; size must equal width * height * 4.
        virtual void SetData(const void* data, uint32_t size) = 0;

        virtual bool operator==(const Texture& other) const = 0;
    };

    class ST_API Texture2D : public Texture
    {
    public:
        // Empty RGBA8 texture to be filled later with SetData.
        static Ref<Texture2D> Create(uint32_t width, uint32_t height);

        // Loads an image file (png, jpg, bmp, tga, ...) via stb_image.
        static Ref<Texture2D> Create(const std::string& path);
    };
}
