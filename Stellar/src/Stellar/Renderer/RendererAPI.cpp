#include "Stellar/Renderer/RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Stellar {

    static RendererAPI::API s_API = RendererAPI::API::OpenGL;

    RendererAPI::API RendererAPI::GetAPI()
    {
        return s_API;
    }

    Scope<RendererAPI> RendererAPI::Create()
    {
        switch (s_API)
        {
            case API::OpenGL: return CreateScope<OpenGLRendererAPI>();
            default: break;
        }

        ST_ASSERT(false, "Unsupported RendererAPI!");
        return nullptr;
    }

}
