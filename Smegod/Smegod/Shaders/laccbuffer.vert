#version 330 core

in vec3 Vertex;

uniform mat4 view_projection;
uniform mat4 world;

void main()
{
    gl_Position = view_projection * world * vec4(Vertex, 1.0);
}