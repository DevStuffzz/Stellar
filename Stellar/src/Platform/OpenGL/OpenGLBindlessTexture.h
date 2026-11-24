#pragma once

#include <glad/glad.h>

// GL_ARB_bindless_texture is not part of the glad loader this project vendors,
// so its entry points are loaded manually once a context is current
// (see OpenGLContext::Init).
namespace Stellar { namespace GLBindless {

    extern GLuint64 (APIENTRY* GetTextureHandleARB)(GLuint texture);
    extern void (APIENTRY* MakeTextureHandleResidentARB)(GLuint64 handle);
    extern void (APIENTRY* MakeTextureHandleNonResidentARB)(GLuint64 handle);
    extern void (APIENTRY* ProgramUniformHandleui64ARB)(GLuint program, GLint location, GLuint64 value);

    // Loads the extension entry points from the current context.
    // Returns true when GL_ARB_bindless_texture is supported.
    bool Init();
    bool IsSupported();

} }
