#version 330 core
in vec2 tex_coord;

layout (location = 0) out vec4 geometry_diffuse;
layout (location = 1) out vec4 geometry_normal_and_specular;
layout (location = 2) out vec4 bloom_filter;
layout (location = 3) out vec4 light;

uniform sampler2D bump;
uniform samplerCube skybox;

uniform float time;

uniform vec3 camera_pos;

in vec3 fN;
in vec3 fB;
in vec3 fT;
in vec3 fL;
in vec3 fV;

vec4 color_deep = vec4(0,0,0.1,1);
vec4 color_shallow = vec4(0,0.2,0.2,1);

vec2 tex_scale = vec2(8, 4);
vec2 bump_speed = vec2(-0.05, 0.0);

float R0 = pow((1.0 - 1.33)/(1.0 + 1.33), 2);

vec3 animateBump(float bump_time, float scalef, float speedf)
{
    vec2 bump_coord = vec2( (tex_coord.xy * tex_scale * scalef) + (bump_time * bump_speed * speedf) );

    return texture(bump, bump_coord).rgb * 2 - 1;
}

const vec3 ambient = vec3(0.1);
void main()
{
    float bump_time = mod(time, 100.0);
    vec3 n_bump = normalize(animateBump(bump_time, 1.0, 1.0) + animateBump(bump_time, 2.0, 4.0) + animateBump(bump_time, 4.0, 8.0));

	vec3 N =  normalize(fN);

    vec3 B = normalize(fB);
    vec3 V = normalize(fV);
    vec3 T = normalize(fT);
    vec3 L = normalize(fL);
    mat3 BTN = mat3(B, T, N);


    N = BTN * n_bump;

    float fresnel = R0 + (1-R0) * pow(1 - dot(V,N), 5) + 0.1;

    vec3 R = normalize(reflect(-V,N));


    vec4 color_water = mix(color_deep, color_shallow, 1-max(dot(V,N),0));
	color_water.rgb *= ambient;
    vec4 reflection = texture(skybox, R);
    vec4 refraction = texture(skybox, refract(-V, N, 1/1.33));

	geometry_normal_and_specular = vec4(N * 0.5 + 0.5, 1);
	light = (reflection * (fresnel)) + (refraction * (1 - fresnel));
	geometry_diffuse = color_water;
}