#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 view;
uniform mat4 projection;

out vec3 texCoords;

void main() {
	gl_Position = projection * mat4(mat3(view)) * vec4(position, 1.0);
	texCoords = position;
}