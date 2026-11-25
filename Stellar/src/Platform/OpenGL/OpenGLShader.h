#pragma once

#include "Stellar/Renderer/Shader.h"

#include <unordered_map>

namespace Stellar {

    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~OpenGLShader();

        void Bind() const override;
        void Unbind() const override;

        void SetInt(const std::string& name, int value) override;
        void SetFloat(const std::string& name, float value) override;
        void SetFloat2(const std::string& name, const glm::vec2& value) override;
        void SetFloat3(const std::string& name, const glm::vec3& value) override;
        void SetFloat4(const std::string& name, const glm::vec4& value) override;
        void SetMat4(const std::string& name, const glm::mat4& value) override;
        void SetTextureHandle(const std::string& name, uint64_t handle) override;

        const std::string& GetName() const override { return m_Name; }
        bool IsValid() const { return m_RendererID != 0; }

    private:
        int GetUniformLocation(const std::string& name);

    private:
        uint32_t m_RendererID = 0;
        std::string m_Name;
        std::unordered_map<std::string, int> m_UniformLocationCache;
    };

}
