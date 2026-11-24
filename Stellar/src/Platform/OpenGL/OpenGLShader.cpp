#include "Platform/OpenGL/OpenGLShader.h"

#include "Platform/OpenGL/OpenGLBindlessTexture.h"

#include <vector>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Stellar {

    static GLuint CompileShader(GLenum type, const std::string& source, const std::string& shaderName)
    {
        GLuint shader = glCreateShader(type);
        const GLchar* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint compiled = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (compiled == GL_FALSE)
        {
            GLint length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::vector<GLchar> log(length > 0 ? length : 1);
            glGetShaderInfoLog(shader, length, &length, log.data());

            std::fprintf(stderr, "[OpenGL] %s shader compilation failed for '%s':\n%s\n",
                type == GL_VERTEX_SHADER ? "Vertex" : "Fragment", shaderName.c_str(), log.data());
            glDeleteShader(shader);
            ST_ASSERT(false, "Shader compilation failure!");
            return 0;
        }

        return shader;
    }

    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
        : m_Name(name)
    {
        GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSrc, name);
        GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc, name);

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        GLint linked = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (linked == GL_FALSE)
        {
            GLint length = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            std::vector<GLchar> log(length > 0 ? length : 1);
            glGetProgramInfoLog(program, length, &length, log.data());

            std::fprintf(stderr, "[OpenGL] Shader link failed for '%s':\n%s\n", name.c_str(), log.data());
            glDeleteProgram(program);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            ST_ASSERT(false, "Shader link failure!");
            return;
        }

        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        m_RendererID = program;
    }

    OpenGLShader::~OpenGLShader()
    {
        glDeleteProgram(m_RendererID);
    }

    void OpenGLShader::Bind() const
    {
        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind() const
    {
        glUseProgram(0);
    }

    int OpenGLShader::GetUniformLocation(const std::string& name)
    {
        auto it = m_UniformLocationCache.find(name);
        if (it != m_UniformLocationCache.end())
            return it->second;

        int location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1)
            std::fprintf(stderr, "[OpenGL] Uniform '%s' not found in shader '%s'\n", name.c_str(), m_Name.c_str());

        m_UniformLocationCache[name] = location;
        return location;
    }

    void OpenGLShader::SetInt(const std::string& name, int value)
    {
        glProgramUniform1i(m_RendererID, GetUniformLocation(name), value);
    }

    void OpenGLShader::SetFloat(const std::string& name, float value)
    {
        glProgramUniform1f(m_RendererID, GetUniformLocation(name), value);
    }

    void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
    {
        glProgramUniform2f(m_RendererID, GetUniformLocation(name), value.x, value.y);
    }

    void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
    {
        glProgramUniform3f(m_RendererID, GetUniformLocation(name), value.x, value.y, value.z);
    }

    void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
    {
        glProgramUniform4f(m_RendererID, GetUniformLocation(name), value.x, value.y, value.z, value.w);
    }

    void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
    {
        glProgramUniformMatrix4fv(m_RendererID, GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void OpenGLShader::SetTextureHandle(const std::string& name, uint64_t handle)
    {
        ST_ASSERT(GLBindless::IsSupported(), "GL_ARB_bindless_texture is not supported by this driver!");
        if (!GLBindless::IsSupported())
            return;

        GLBindless::ProgramUniformHandleui64ARB(m_RendererID, GetUniformLocation(name), handle);
    }

}
