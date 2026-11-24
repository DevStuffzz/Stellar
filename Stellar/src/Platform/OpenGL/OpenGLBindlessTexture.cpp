#include "Platform/OpenGL/OpenGLBindlessTexture.h"

#include <cstring>

#include <GLFW/glfw3.h>

namespace Stellar { namespace GLBindless {

    GLuint64 (APIENTRY* GetTextureHandleARB)(GLuint) = nullptr;
    void (APIENTRY* MakeTextureHandleResidentARB)(GLuint64) = nullptr;
    void (APIENTRY* MakeTextureHandleNonResidentARB)(GLuint64) = nullptr;
    void (APIENTRY* ProgramUniformHandleui64ARB)(GLuint, GLint, GLuint64) = nullptr;

    static bool s_Supported = false;

    static bool HasExtension(const char* name)
    {
        GLint count = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &count);
        for (GLint i = 0; i < count; i++)
        {
            const char* extension = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, static_cast<GLuint>(i)));
            if (extension && std::strcmp(extension, name) == 0)
                return true;
        }
        return false;
    }

    bool Init()
    {
        s_Supported = false;

        if (!HasExtension("GL_ARB_bindless_texture"))
            return false;

        GetTextureHandleARB = reinterpret_cast<GLuint64(APIENTRY*)(GLuint)>(
            glfwGetProcAddress("glGetTextureHandleARB"));
        MakeTextureHandleResidentARB = reinterpret_cast<void(APIENTRY*)(GLuint64)>(
            glfwGetProcAddress("glMakeTextureHandleResidentARB"));
        MakeTextureHandleNonResidentARB = reinterpret_cast<void(APIENTRY*)(GLuint64)>(
            glfwGetProcAddress("glMakeTextureHandleNonResidentARB"));
        ProgramUniformHandleui64ARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint64)>(
            glfwGetProcAddress("glProgramUniformHandleui64ARB"));

        s_Supported = GetTextureHandleARB && MakeTextureHandleResidentARB
            && MakeTextureHandleNonResidentARB && ProgramUniformHandleui64ARB;
        return s_Supported;
    }

    bool IsSupported()
    {
        return s_Supported;
    }

} }
