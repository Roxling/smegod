#version 330 core

uniform sampler2D depthBuffer;
uniform sampler2D normalAndSpecularBuffer;
uniform sampler2DShadow shadowMap;

uniform vec2 invRes;


uniform mat4 view;
uniform mat4 projection;

uniform vec3 camera_pos;
uniform mat4 shadowViewProjection;

uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 light_dir;
uniform float light_intensity;
uniform float light_anglefalloff;

uniform vec2 shadow_texelsize;

layout (location = 0) out vec4 light_contribution;

void main()
{
    mat4 ViewProjection =  projection * view;
    mat4 ViewProjectionInverse = inverse(ViewProjection);

    vec2 screen_coord = gl_FragCoord.xy * invRes; // [0,1]
    vec2 ndc = screen_coord * 2 - 1;

    float depth = texture(depthBuffer, screen_coord).r * 2 - 1;


    //Composite light using phong shading, falloffs and LightIntensity and LightColor

    vec4 ndc_pos = vec4(ndc, depth, 1);
    vec4 pixel_world = (ViewProjectionInverse * ndc_pos);
    pixel_world.xyz /= pixel_world.w;


    float length = distance(light_pos, pixel_world.xyz);
  

  

    vec4 NnS = texture(normalAndSpecularBuffer, screen_coord);
    vec3 N = NnS.xyz*2 - 1;
    float shininess = NnS.a*100;
	if(NnS.a == 0) // if shininess is 0 then specular (pow(x,0) -> 1) and fully visible.
		shininess = 1;

    N = normalize(N);
    vec3 L = normalize(light_pos - pixel_world.xyz);
    vec3 V = normalize(camera_pos - pixel_world.xyz);
    vec3 R = normalize(reflect(-L,N));


    float rad = acos(dot(L, normalize(light_dir)));
    if(rad > light_anglefalloff)
        rad = light_anglefalloff;

    float radialFalloff = 1 - rad/light_anglefalloff;
	float distfalloff = 1 / pow(length, 2);

	float intensityscale = 10000;

	vec3 color = light_color * radialFalloff * distfalloff * (light_intensity / intensityscale);
   
    vec3 diffuse = color * max(dot(L,N),0.0);
    vec3 specular = color *2 * pow(max(dot(V,R),0.0), shininess);

    light_contribution.xyz = diffuse+specular;
    light_contribution.w = 1;
}