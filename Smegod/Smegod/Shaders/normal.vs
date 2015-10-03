#version 330 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 world;

uniform mat4 worldIT;

out vec3 fN;

void main()
{
    gl_Position = projection * view * world * vec4(position, 1.0f);
	fN = (worldIT * vec4(normal,0)).xyz;
}