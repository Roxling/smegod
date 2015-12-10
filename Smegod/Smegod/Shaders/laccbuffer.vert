#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 u_view_projection;
uniform mat4 u_world;

void main()
{
    gl_Position = u_view_projection * u_world * vec4(position, 1.0);
}