#include "Stellar/Renderer/Shader.h"

#include "Stellar/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Stellar {

    Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::OpenGL: return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
            default: break;
        }

        ST_ASSERT(false, "Unsupported RendererAPI!");
        return nullptr;
    }

}
