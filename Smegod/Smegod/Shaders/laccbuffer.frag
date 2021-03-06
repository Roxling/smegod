#version 420 core

uniform sampler2D depthBuffer;
uniform sampler2D normalAndSpecularBuffer;
uniform sampler2D shadowMap;
uniform sampler2D diffuseBuffer;

uniform vec2 u_inv_res;
uniform mat4 u_view_projection_inverse;
uniform vec3 u_camera_pos;
uniform mat4 u_world_to_light;
uniform vec2 u_shadow_texelsize;

uniform float u_specular_power;

layout (location = 0) out vec4 out_light_contribution;
layout (location = 1) out vec4 out_bloom_filter;

#define saturate(a) clamp( a, 0.0, 1.0 )

struct Light {
	vec3 position;
	vec3 direction;
	vec3 color; // Color and intensity

	float cutOff;
	float outerCutOff;

	float linear;
	float quadratic;
};
uniform Light u_light;


void main()
{
    vec2 screen_coord = gl_FragCoord.xy * u_inv_res; // [0,1]
    vec2 ndc = screen_coord * 2 - 1; // [-1,1]

	// Calculate fragments position in world space
    float depth = texture(depthBuffer, screen_coord).r * 2 - 1; // [-1,1]
    vec4 ndc_pos = vec4(ndc, depth, 1);
    vec4 pixel_pos_world = u_view_projection_inverse * ndc_pos;
    pixel_pos_world /= pixel_pos_world.w;

	// Calculate vectors
	vec4 NnS = texture(normalAndSpecularBuffer, screen_coord);
    vec3 N = NnS.xyz * 2 - 1;

    N = normalize(N);
    vec3 L = normalize(u_light.position - pixel_pos_world.xyz);
    vec3 V = normalize(u_camera_pos - pixel_pos_world.xyz);
    vec3 H = normalize(L + V);

    // Shadow
    vec4 pixel_in_light = u_world_to_light * pixel_pos_world;
    pixel_in_light /= pixel_in_light.w; // [-1, 1]
	pixel_in_light = pixel_in_light * 0.5 + 0.5; // [0, 1]


    float bias = max(0.0002 * (1.0 - dot(N, L)), 0.0001);
	bias = 0;
    float shadow_factor = 0.0;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, pixel_in_light.xy + vec2(x, y) * u_shadow_texelsize).r; // [0, 1]
            shadow_factor += (pixel_in_light.z - bias) < pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow_factor /= 9.0;

    // Blinn-Phong

	// Radial attenuation
    float theta = dot(L, normalize(u_light.direction)); 
    float epsilon = (u_light.cutOff - u_light.outerCutOff);
    float intensity = saturate((theta - u_light.outerCutOff) / epsilon);

	// Distance attenuation
	float la = u_light.linear;
	float qa = u_light.quadratic;
	//la = 0.02;
	//qa  =0.002;
	float intens = 1;// .5;
	float dist =  distance(u_light.position, pixel_pos_world.xyz);
    float attenuation = 1.0 / (1.0 + la * dist + qa * (dist * dist));

	// Diffuse color
    vec3 diffuse = texture(diffuseBuffer, screen_coord).rgb * u_light.color * intens * max(dot(L,N), 0.0);;

	// Specular color
	float specular_factor = NnS.a * 2;
    vec3 specular = specular_factor * u_light.color * intens * pow(max(dot(N, H), 0.0), u_specular_power);

	// Final light
	float total_intensity = intensity * attenuation * shadow_factor;
    vec3 full_color = (diffuse + specular) * total_intensity;

	// Send "over bright" regions to the bloom filter
	out_light_contribution = vec4(full_color, 1.0);
	//out_light_contribution = vec4(vec3(total_intensity, total_intensity, 1.0), 1.0);
	//out_light_contribution = vec4(normalize(-u_light.direction)*0.5 + 0.5, 1.0);
	
    float brightness = dot(full_color, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        out_bloom_filter = vec4(full_color, 1.0);
}