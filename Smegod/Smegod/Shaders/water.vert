#version 330 core
layout (location = 0) in vec3 position;
out vec2 tex_coord;

uniform mat4 view_projection;
uniform mat4 world;

uniform float time;

uniform vec3 light_pos;
uniform vec3 camera_pos;

out vec3 fN;
out vec3 fB;
out vec3 fT;
out vec3 fV;
out vec3 fL;


float G(float ampl, float dirx, float dirz, float freq, float phase, float sharp, float time)
{
	return ampl * pow(( sin((dirx*position.x + dirz*position.z)*freq +time*phase)* 0.5 + 0.5),sharp);
}
float dGdx(float ampl, float dirx, float dirz, float freq, float phase, float sharp, float time)
{
	return 0.5 * sharp * freq * ampl * pow(( sin((dirx*position.x + dirz*position.z)*freq +time*phase)* 0.5 + 0.5),sharp-1) * cos((dirx * position.x + dirz * position.z)*freq + time*phase)*dirx;
}

float dGdz(float ampl, float dirx, float dirz, float freq, float phase, float sharp, float time)
{
	return 0.5 * sharp * freq * ampl * pow(( sin((dirx*position.x + dirz*position.z)*freq +time*phase)* 0.5 + 0.5),sharp-1) * cos((dirx * position.x + dirz * position.z)*freq + time*phase)*dirz;
}

vec3 wave(float ampl, float dirx, float dirz, float freq, float phase, float sharp, float time){
	vec3 v;
	v.x = G(ampl, dirx, dirz, freq, phase, sharp, time);
	v.y = dGdx(ampl, dirx, dirz, freq, phase, sharp, time);
	v.z = dGdz(ampl, dirx, dirz, freq, phase, sharp, time);
	return v;
}

void main()
{
	mat4 worldIT = transpose(inverse(world));

	vec3 w1 = wave(0.8,	-1,		1,		0.2,	0.5,	2,	time);
	vec3 w2 = wave(0.5,	-0.6,	0.7,	0.4,	1.3,	4,	time);
	vec3 w3 = wave(0.2,	-0.8,	0.4,	0.1,	0.3,	6,	time);
	vec3 w4 = wave(0.3,	-1,		0.4,	0.8,	1.7,	10,	time);
	vec3 tide = wave(6,	-1,	1,	0.01,	0.1,	100, time);
	vec3 w = w1 + w2 + w3 + w4 + tide;


	vec3 pos = position;
	pos.y = w.x;

    gl_Position = view_projection  * world * vec4(pos, 1.0);

	vec3 norm = vec3( -w.y,1, -w.z);
	vec3 b = vec3(1.0, w.y, 0.0);
	vec3 t = vec3(0.0, w.z, 1.0);
	tex_coord = position.xz/100;

	vec3 world_pos = (world*vec4(position,1)).xyz;
	fN = (worldIT * vec4(norm,0)).xyz;
	fB = (worldIT * vec4(b,0)).xyz;
	fT = (worldIT * vec4(t,0)).xyz;
	fV = camera_pos - world_pos;
	fL = light_pos - world_pos;
}