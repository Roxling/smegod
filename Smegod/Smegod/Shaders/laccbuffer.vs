#version 330 core

in vec3 Vertex;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 world;

void main()
{
    gl_Position = projection * view * world * vec4(Vertex, 1.0f);
}