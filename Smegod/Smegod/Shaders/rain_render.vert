#version 330 core
layout (location = 0) in vec3 pos0;
layout (location = 1) in vec3 seed0;
layout (location = 2) in vec3 speed0;
layout (location = 3) in float random0;
layout (location = 4) in uint type0;

out vec3 pos;
out vec3 seed;
out vec3 speed;
out float random;
out uint type;

void main()
{
	pos = pos0;
	seed = seed0;
	speed = speed0;
	random = random0;
	type = type0;
}