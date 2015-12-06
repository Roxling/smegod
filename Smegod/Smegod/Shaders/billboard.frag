#version 330 core
uniform sampler2D tex;

in vec2 texcoords;
out vec4 frag_color;

void main()
{
    frag_color = texture(tex, texcoords);

    if (frag_color.r == 0 && frag_color.g == 0 && frag_color.b == 0) {
        discard;
    }
}