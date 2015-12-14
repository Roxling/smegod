#version 420 core
layout (location = 0) in vec3 position;

uniform mat4 view;
uniform mat4 projection;

out vec3 texCoords;

void main() {
	vec4 pos = projection * mat4(mat3(view)) * vec4(position, 1.0);
	gl_Position = pos.xyww;
	texCoords = position;
}