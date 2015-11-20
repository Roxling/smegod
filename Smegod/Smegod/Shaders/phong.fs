#version 330 core
in vec2 tex_coord;

out vec4 color;
uniform vec3 incolor;
uniform sampler2D tex;
uniform sampler2D bump;

uniform mat4 worldIT;

uniform vec3 kambient;
uniform vec3 kdiffuse;
uniform vec3 kspecular;
uniform float shininess;

in vec3 fN;
in vec3 fB;
in vec3 fT;
in vec3 fL;
in vec3 fV;

void main()
{
    vec4 text = texture(tex, tex_coord);
	if(text.a < 0.01)
        discard;
    vec3 bump = texture(bump, tex_coord).rgb;
	bump = bump * 2 - 1;

	vec3 N =  fT * bump.x + fB * bump.y + fN * bump.z;

    N = normalize(N);
    vec3 L = normalize(fL);
    vec3 V = normalize(fV);
    vec3 R = normalize(reflect(-L,N));
    vec3 diffuse = kdiffuse * max(dot(L,N),0.0);
    vec3 specular = kspecular * pow(max(dot(V,R),0.0), shininess);
    color.xyz = (kambient + diffuse + specular)*text.rgb;
    color.w = 1; 
} 