#version 330 core
layout (location = 0) in vec3 position;

out vec2 tex_cord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 world;

void main()
{
    gl_Position = projection * view * world * vec4(position, 1.0f);
	tex_cord = vec2(position.x + position.y, position.z);
}