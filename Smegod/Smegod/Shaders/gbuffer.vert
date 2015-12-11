#version 420 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 texture_coord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 binormal;
layout (location = 4) in vec3 tangent;

out vec3 worldspace_normal;
out vec3 worldspace_binormal;
out vec3 worldspace_tangent;
out vec2 pass_texcoords;

uniform mat4 world;
uniform mat4 view_projection;

void main()
{
	mat4 worldIT = transpose(inverse(world));
    worldspace_normal   = normalize((worldIT * vec4(normal, 0.0)).xyz);
	worldspace_tangent  = normalize((worldIT * vec4(tangent, 0.0)).xyz);
	worldspace_binormal = normalize((worldIT * vec4(binormal, 0.0)).xyz);
    pass_texcoords = texture_coord.xy;

    gl_Position = view_projection * world * vec4(position, 1.0);
}