#version 420 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_seed;
layout (location = 2) in vec3 a_speed;
layout (location = 3) in float a_random;
layout (location = 4) in float a_type;

out vec3 vs_pos;
out vec3 vs_seed;
out vec3 vs_speed;
out float vs_random;
out float vs_type;

void main()
{
	vs_pos = a_pos;
	vs_seed = a_seed;
	vs_speed = a_speed;
	vs_random = a_random;
	vs_type = a_type;
}