#version 330 core
in vec2 tex_coord;

out vec4 color;

uniform sampler2D bump;


uniform vec3 kambient;
uniform vec3 kdiffuse;
uniform vec3 kspecular;
uniform float shininess;

in vec3 fN;
in vec3 fB;
in vec3 fT;
in vec3 fL;
in vec3 fV;

vec4 color_deep = vec4(0,0,0.1,1);
vec4 color_shallow = vec4(0,0.5,0.5,1);

void main()
{
	vec3 bump = texture(bump, tex_coord).rgb;
	bump = bump * 2 - 1;

	vec3 N =  normalize(fN);

    vec3 L = normalize(fL);
    vec3 V = normalize(fV);
    vec3 R = normalize(reflect(-L,N));
   
    color = mix(color_deep, color_shallow, 1-max(dot(V,N),0));

} 