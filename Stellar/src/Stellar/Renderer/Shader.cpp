#include "Stellar/Renderer/Shader.h"

#include "Stellar/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include "Stellar/Util/FileUtil.h"

#include <cstdio>
#include <sstream>
#include <stdexcept>


namespace Stellar {

    Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::OpenGL:
            {
                auto shader = CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
                return shader->IsValid() ? shader : nullptr;
            }
            default: break;
        }

        ST_ASSERT(false, "Unsupported RendererAPI!");
        return nullptr;
    }


    Ref<Shader> Shader::Load(const std::string& name, const std::string& filename)
    {
        std::string vertexSrc, fragmentSrc;
        std::string file;
        try
        {
            file = File::ReadText(filename);
        }
        catch (const std::runtime_error& error)
        {
            std::fprintf(stderr, "[Shader] %s\n", error.what());
            return nullptr;
        }

        std::istringstream stream(file);
        std::string line;
        std::string* currentSrc = nullptr;

        while (std::getline(stream, line))
        {
            // Remove carriage returns from Windows line endings
            if (!line.empty() && line.back() == '\r')
                line.pop_back();

            if (line.rfind("#type ", 0) == 0)
            {
                std::string type = line.substr(6);

                if (type == "vertex")
                    currentSrc = &vertexSrc;
                else if (type == "fragment")
                    currentSrc = &fragmentSrc;
                else
                {
                    std::fprintf(stderr, "[Shader] Unknown shader type '%s' in '%s'\n", type.c_str(), filename.c_str());
                    return nullptr;
                }

                continue;
            }

            if (currentSrc)
                *currentSrc += line + '\n';
        }

        if (vertexSrc.empty() || fragmentSrc.empty())
        {
            std::fprintf(stderr, "[Shader] Missing vertex or fragment source in '%s'\n", filename.c_str());
            return nullptr;
        }

        return Create(name, vertexSrc, fragmentSrc);
    }
}
