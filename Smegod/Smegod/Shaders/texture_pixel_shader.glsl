#version 330 core
in vec2 tex_cord;

out vec4 color;
uniform vec3 incolor;
uniform sampler2D tex;

void main()
{
    color = (2 * texture(tex, tex_cord) + vec4(incolor, 1.0f))/3;
} 