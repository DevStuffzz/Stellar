#include "Stellar/Renderer/Buffer.h"

#include "Stellar/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Stellar {

    Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(size);
            default: break;
        }

        ST_ASSERT(false, "Unsupported RendererAPI!");
        return nullptr;
    }

    Ref<VertexBuffer> VertexBuffer::Create(const void* vertices, uint32_t size)
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(vertices, size);
            default: break;
        }

        ST_ASSERT(false, "Unsupported RendererAPI!");
        return nullptr;
    }

    Ref<IndexBuffer> IndexBuffer::Create(const uint32_t* indices, uint32_t count)
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::OpenGL: return CreateRef<OpenGLIndexBuffer>(indices, count);
            default: break;
        }

        ST_ASSERT(false, "Unsupported RendererAPI!");
        return nullptr;
    }

}
