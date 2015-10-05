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

uniform mat4 worldIT;


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
	vec3 w1 = wave(10.0,-1.0,0.0,0.2,0.5,2,time);
	vec3 w2 = wave(0.5,-0.7,0.7,0.4,1.3,2,time);

	vec3 pos = position;
	pos.y =  w1.x + w2.x;

    gl_Position = projection * view * world * vec4(pos, 1.0);

	vec3 norm = vec3( -(w1.y + w2.y) ,1, -(w1.z + w2.z));
	tex_coord = texture_coord.xy;

	vec3 world_pos = (world*vec4(position,1)).xyz;
	fN = (worldIT * vec4(norm,0)).xyz;
	fB = binormal;
	fT = tangent;
	fV = camera_pos - world_pos;
	fL = light_pos - world_pos;

}