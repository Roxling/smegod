#version 330 core
in vec2 tex_coord;

out vec4 color;
uniform vec3 incolor;
uniform sampler2D tex;

void main()
{
    color = (2 * texture(tex, tex_coord) + vec4(incolor, 1.0f))/3;
} 