#version 330 core

out vec4 color;
in vec3 fN;

void main()
{
	vec3 N = normalize(fN);
    color = vec4(N/2 + 0.5, 1.0f);
} 