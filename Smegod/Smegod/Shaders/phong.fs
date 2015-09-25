#version 330 core
in vec2 tex_coord;

out vec4 color;
uniform vec3 incolor;
uniform sampler2D tex;

uniform vec3 kambient;
uniform vec3 kdiffuse;
uniform vec3 kspecular;
uniform float shininess;

in vec3 fN;
in vec3 fL;
in vec3 fV;

void main()
{
    vec3 texture = texture(tex, tex_coord).xyz;
    vec3 N = normalize(fN);
    vec3 L = normalize(fL);
    vec3 V = normalize(fV);
    vec3 R = normalize(reflect(-L,N));
    vec3 diffuse = kdiffuse*max(dot(L,N),0.0);
    vec3 specular =kspecular*pow(max(dot(V,R),0.0), shininess);
    color.xyz = (kambient + diffuse + specular)*texture*incolor;
    color.w = 1.0; 
} 