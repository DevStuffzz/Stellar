#include "Stellar/Renderer/VertexArray.h"

#include "Stellar/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Stellar {

    Ref<VertexArray> VertexArray::Create()
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexArray>();
            default: break;
        }

        ST_ASSERT(false, "Unsupported RendererAPI!");
        return nullptr;
    }

}
