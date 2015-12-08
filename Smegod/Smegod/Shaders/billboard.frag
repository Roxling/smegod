#version 330 core
uniform sampler2D tex;

in vec2 gs_tex;
out vec4 frag_color;

void main()
{
    frag_color = texture(tex, gs_tex);
}