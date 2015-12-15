#version 420 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 texture_coord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 binormal;
layout (location = 4) in vec3 tangent;

out vec4 worldspace_position;
out vec3 worldspace_normal;
out vec3 worldspace_binormal;
out vec3 worldspace_tangent;
out vec2 pass_texcoords;

out vec3 tangent_view_position;
out vec3 tangent_frag_position;

uniform mat4 world;
uniform mat4 view_projection;
uniform vec3 u_camera_pos;

void main()
{
	mat4 worldIT = transpose(inverse(world));
    worldspace_normal   = normalize((worldIT * vec4(normal, 0.0)).xyz);
	worldspace_tangent  = normalize((worldIT * vec4(tangent, 0.0)).xyz);
	worldspace_binormal = normalize((worldIT * vec4(binormal, 0.0)).xyz);
    pass_texcoords = texture_coord.xy;

	worldspace_position =  world * vec4(position, 1.0);
    gl_Position = view_projection * worldspace_position;

    mat3 TBN = transpose(mat3(worldspace_tangent, worldspace_binormal, worldspace_normal));

    tangent_view_position  = TBN * u_camera_pos;
    tangent_frag_position  = TBN * vec3(worldspace_position);
}