#version 420 core

uniform mat4 u_world_to_light;
uniform mat4 u_world;

layout (location = 0) in vec3 position;

void main()
{
	gl_Position = u_world_to_light * u_world * vec4(position, 1.0);
}