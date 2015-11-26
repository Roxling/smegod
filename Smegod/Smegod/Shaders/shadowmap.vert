#version 330 core

uniform mat4 model_to_clip_matrix;
uniform mat4 world;

layout (location = 0) in vec3 position;

void main()
{
	gl_Position = model_to_clip_matrix * world * vec4(position, 1.0);
}