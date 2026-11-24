#include "Stellar/Renderer/RenderTarget.h"

#include "Platform/OpenGL/OpenGLRenderTarget.h"
#include "Stellar/Renderer/RendererAPI.h"

#include <stdexcept>

namespace Stellar {

    Ref<RenderTarget> RenderTarget::Create(uint32_t width, uint32_t height)
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::OpenGL: return CreateRef<OpenGLRenderTarget>(width, height);
            default: throw std::runtime_error("Unsupported renderer API for render target");
        }
    }

}
