#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 seed;
layout (location = 2) in vec3 speed;
layout (location = 3) in float random;
layout (location = 4) in uint type;

out VS_OUT {
	vec3 pos;
	vec3 seed;
	vec3 speed;
	float random;
	uint type;
} vs_out;

void main()
{
	vs_out.pos = pos;
	vs_out.seed = seed;
	vs_out.speed = speed;
	vs_out.random = random;
	vs_out.type = type;
}