#version 330 core

uniform sampler2D depthBuffer;
uniform sampler2D normalAndSpecularBuffer;
uniform sampler2D shadowMap;

uniform vec2 invRes;


uniform mat4 view;
uniform mat4 projection;

uniform vec3 camera_pos;
uniform mat4 worldToLight;

uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 light_dir;
uniform float light_intensity;
uniform float light_anglefalloff;

uniform vec2 shadow_texelsize;

layout (location = 0) out vec4 light_contribution;

#define saturate(a) clamp( a, 0.0, 1.0 )
#define whiteCompliment(a) ( 1.0 - saturate( a ) )

float calcLightAttenuation( float lightDistance, float cutoffDistance, float decayExponent ) {
	if ( decayExponent > 0.0 ) {
	  return pow( saturate( -lightDistance / cutoffDistance + 1.0 ), decayExponent );
	}
	return 1.0;
}


void main()
{
    vec2 screen_coord = gl_FragCoord.xy * invRes; // [0,1]
    vec2 ndc = screen_coord * 2 - 1;
    mat4 ViewProjection =  projection * view;
    mat4 ViewProjectionInverse = inverse(ViewProjection);

    float depth = texture(depthBuffer, screen_coord).r * 2 - 1;
    vec4 ndc_pos = vec4(ndc, depth, 1);
    vec4 pixel_world = ViewProjectionInverse * ndc_pos;
    pixel_world /= pixel_world.w;

    //SHADOW
    vec4 pixel_in_light = worldToLight * pixel_world;
    pixel_in_light /= pixel_in_light.w;

    float shadowdepth = texture(shadowMap, pixel_in_light.xy*0.5 +0.5).r * 2 - 1;
    //float shadowdepth = texture(shadowMap, pixel_in_light.xyz);

    

   // float shadow = 0;
    //if(shadowdepth > pixel_in_light.z)
    //    shadow = 1;
    
 


    // PHONG
    //Composite light using phong shading, falloffs and LightIntensity and LightColor


    float length = distance(light_pos, pixel_world.xyz);
  
    vec4 NnS = texture(normalAndSpecularBuffer, screen_coord);
    vec3 N = NnS.xyz*2 - 1;
    float shininess = NnS.a*100;

    N = normalize(N);
    vec3 L = normalize(light_pos - pixel_world.xyz);
    vec3 V = normalize(camera_pos - pixel_world.xyz);
    vec3 H = normalize(L + V);

    float bias = max(0.05 * (1.0 - dot(N, L)), 0.005);
    bias = 0;
    float shadow = 0.0;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, pixel_in_light.xy*0.5 +0.5 + vec2(x, y) * shadow_texelsize).r * 2 - 1; 
            shadow +=(pixel_in_light.z - bias) < pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    float rad = acos(dot(L, normalize(light_dir)));
    if(rad > light_anglefalloff)
        rad = light_anglefalloff;

    float radialFalloff = 1 - rad/light_anglefalloff;
	float distfalloff = 1 / (length * length);

	float intensityscale = 1000;

	vec3 color = light_color * radialFalloff * distfalloff * (light_intensity / intensityscale);
   
    vec3 diffuse = color * max(dot(L,N),0.0);
    vec3 specular = color * pow(max(dot(N,H),0.0), shininess);
    if(NnS.a == 0)
        specular = vec3(0);

    light_contribution.xyz = (diffuse + specular)*shadow;
    light_contribution.w = 1;

    //TO REMOVE
    //light_contribution.xyz = normalize(pixel_in_light.xyz)*0.5 + 0.5;
   // light_contribution.xyz = vec3(shadowdepth);
    //light_contribution.xyz = light_color*shadow;
}