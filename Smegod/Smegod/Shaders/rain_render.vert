#version 330 core
in vec3 a_pos;
in vec3 a_seed;
in vec3 a_speed;
in float a_random;
in uint a_type;

out vec3 vs_pos;
out vec3 vs_seed;
out vec3 vs_speed;
out float vs_random;
out uint vs_type;

void main()
{
	vs_pos = a_pos;
	vs_seed = a_seed;
	vs_speed = a_speed;
	vs_random = a_random;
	vs_type = a_type;
}