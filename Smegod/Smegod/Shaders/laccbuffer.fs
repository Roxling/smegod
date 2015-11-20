#version 330 core

uniform sampler2D gDiffuse;
uniform sampler2D gNormal;
uniform sampler2D gDepth;

layout (location = 3) out vec4 light_acc;
in vec2 pass_texcoords;


void main()
{
    vec4 color = texture(gNormal, pass_texcoords);
    light_acc = color;
} 