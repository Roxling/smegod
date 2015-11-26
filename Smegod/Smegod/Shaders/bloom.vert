#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 texture_coord;

out vec2 pass_texcoords;

void main()
{
    gl_Position = vec4(position, 1.0);
    pass_texcoords = texture_coord.xy;
}