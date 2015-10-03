#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 texture_coord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 binormal;
layout (location = 4) in vec3 tangent;
out vec2 tex_coord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 world;


uniform vec3 light_pos;
uniform vec3 camera_pos;

out vec3 fN;
out vec3 fB;
out vec3 fT;
out vec3 fV;
out vec3 fL;

void main()
{
    gl_Position = projection * view * world * vec4(position, 1.0f);
	tex_coord = texture_coord.xy;

	vec3 world_pos = (world*vec4(position,1)).xyz;
	fN = normal;
	fB = binormal;
	fT = tangent;
	fV = camera_pos - world_pos;
	fL = light_pos - world_pos; 
}