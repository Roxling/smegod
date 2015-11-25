#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 texture_coord;

out vec2 tex_coord;

void main()
{
    gl_Position = vec4(position, 1.0);
	tex_coord = texture_coord.xy;
}