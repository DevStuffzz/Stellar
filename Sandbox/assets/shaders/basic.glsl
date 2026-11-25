#type vertex
#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform vec2 u_UVOffset;
uniform vec2 u_UVScale;

out vec4 v_Color;
out vec2 v_TexCoord;

void main()
{
    v_Color = a_Color;
    v_TexCoord = u_UVOffset + a_TexCoord * u_UVScale;
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 0.0, 1.0);
}

#type fragment
#version 450 core
#extension GL_ARB_bindless_texture : require

layout(location = 0) out vec4 o_Color;

in vec4 v_Color;
in vec2 v_TexCoord;

uniform vec4 u_Color;
layout(bindless_sampler) uniform sampler2D u_Texture;

void main()
{
    o_Color = texture(u_Texture, v_TexCoord) * v_Color * u_Color;
}