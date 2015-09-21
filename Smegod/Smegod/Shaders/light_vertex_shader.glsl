#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 world;

void main()
{
    gl_Position = projection * view * world * vec4(position, 1.0f);
}