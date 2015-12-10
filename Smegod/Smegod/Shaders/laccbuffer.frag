#version 330 core

uniform sampler2D depthBuffer;
uniform sampler2D normalAndSpecularBuffer;
uniform sampler2D shadowMap;
uniform sampler2D diffuseBuffer;

uniform vec2 invRes;


uniform mat4 view_projection;
uniform mat4 view_projection_inverse;

uniform vec3 camera_pos;
uniform mat4 worldToLight;

uniform vec2 shadow_texelsize;

uniform float u_specular_power = 100;

layout (location = 0) out vec4 light_contribution;
layout (location = 1) out vec4 bloom_filter;

#define saturate(a) clamp( a, 0.0, 1.0 )
#define whiteCompliment(a) ( 1.0 - saturate( a ) )


struct Light {
	vec3 position;
	vec3 direction;
	vec3 color;
	float cutOff;
	float outerCutOff;

	float linear;
	float quadratic;
};
uniform Light u_light;


void main()
{
    vec2 screen_coord = gl_FragCoord.xy * invRes; // [0,1]
    vec2 ndc = screen_coord * 2 - 1; // [-1,1]

    float depth = texture(depthBuffer, screen_coord).r * 2 - 1; // [-1,1]
    vec4 ndc_pos = vec4(ndc, depth, 1);
    vec4 pixel_world = view_projection_inverse * ndc_pos;
    pixel_world /= pixel_world.w;

    // Shadow
    vec4 pixel_in_light = worldToLight * pixel_world;
    pixel_in_light /= pixel_in_light.w;

    float shadowdepth = texture(shadowMap, pixel_in_light.xy*0.5 +0.5).r * 2 - 1;

    float bias = max(0.05 * (1.0 - dot(N, L)), 0.005);
    bias = 0;
    float shadow_factor = 0.0;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, pixel_in_light.xy*0.5 +0.5 + vec2(x, y) * shadow_texelsize).r * 2 - 1; 
            shadow_factor += (pixel_in_light.z - bias) < pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow_factor /= 9.0;

    // Blinn-Phong
    vec4 NnS = texture(normalAndSpecularBuffer, screen_coord);
    vec3 N = NnS.xyz*2 - 1;

    N = normalize(N);
    vec3 L = normalize(u_light.position - pixel_world.xyz);
    vec3 V = normalize(camera_pos - pixel_world.xyz);
    vec3 H = normalize(L + V);

	// Radial attenuation
    float theta = dot(L, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = saturate((theta - light.outerCutOff) / epsilon);

	// Distance attenuation
	float dist =  distance(light.position, pixel_world.xyz);
    float attenuation = 1.0f / (1.0 + light.linear * dist + light.quadratic * (dist * dist));

	// Diffuse color
    vec3 diffuse = texture(diffuseBuffer, screen_coord).rgb * u_light.color * max(dot(L,N),0.0);;

	// Specular color
	float specular_factor = NnS.a;
    vec3 specular = specular_factor * u_light.color * pow(max(dot(N,H),0.0), u_specular_power);

	float total_intensity = intensity * attenuation * shadow_factor;
   
	//nått i diffuse och/eller specular
    vec3 full_color = (diffuse + specular)*total_intensity;

	light_contribution = vec4(full_color, 1.0);
    float brightness = dot(full_color, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        bloom_filter = vec4(full_color, 1.0);
}