#include "Stellar/Renderer/GraphicsContext.h"

#include "Stellar/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Stellar {

    Scope<GraphicsContext> GraphicsContext::Create(void* nativeWindow)
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::OpenGL: return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(nativeWindow));
            default: break;
        }

        ST_ASSERT(false, "Unsupported RendererAPI!");
        return nullptr;
    }

}
