#version 330 core

in vec2 tex_coord;
out vec4 color;
uniform sampler2D buff;

void main()
{
    vec4 text = texture(buff, tex_coord);
    color = text;
} 