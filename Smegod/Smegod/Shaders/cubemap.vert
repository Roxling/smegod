#version 420 core
layout (location = 0) in vec3 position;

uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 texcoords;

void main() {
	vec4 pos = u_projection * mat4(mat3(u_view)) * vec4(position, 1.0);
	gl_Position = pos.xyww;
	texcoords = position;
}