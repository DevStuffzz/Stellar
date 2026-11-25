#pragma once

#include "Core.h"

#include <cstdint>
#include <string>

#include <glm/glm.hpp>

namespace Stellar {

    class ST_API Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetInt(const std::string& name, int value) = 0;
        virtual void SetFloat(const std::string& name, float value) = 0;
        virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
        virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
        virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

        // Sets a bindless texture handle on a sampler uniform (see Texture::GetHandle).
        virtual void SetTextureHandle(const std::string& name, uint64_t handle) = 0;

        virtual const std::string& GetName() const = 0;

        // Returns null after reporting a shader compilation, link, or file error.
        static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        static Ref<Shader> Load(const std::string& name, const std::string& filename);
    };

}
