#version 420 core
uniform sampler2DArray rainTextureArray;
layout (location = 0) out vec4 gRain;

struct Light{
	vec3 position;
	vec3 direction;
	vec3 color; // Color and intensity

	float cutOff;
	float outerCutOff;

	float linear;
	float quadratic;
};
#define saturate(a) clamp( a, 0.0, 1.0 )
#define NUM_LIGHTS (6)
uniform Light lights[NUM_LIGHTS];

in vec2 gs_tex;
in vec3 gs_pos;
flat in uint gs_type;
in float gs_random;

float ambient = 0.6;
void main()
{
	
    vec4 color = texture(rainTextureArray, vec3(gs_tex,gs_type)) + texture(rainTextureArray, vec3(gs_tex,gs_random));
	color = normalize(color);
	color.a = 1;
	gRain = ambient * color;
	for(int i = 0; i < NUM_LIGHTS; i++){
		vec3 rD = normalize( lights[i].position - gs_pos );

		float theta = dot(rD, normalize(lights[i].direction)); 
		float epsilon = (lights[i].cutOff - lights[i].outerCutOff);
		float intensity = saturate((theta - lights[i].outerCutOff) / epsilon);

		float la = lights[i].linear;
		float qa = lights[i].quadratic;
		float dist =  distance(lights[i].position, gs_pos);
		float attenuation = 1.0 / (1.0 + la * dist + qa * (dist * dist));

	
		gRain += intensity * attenuation * color * lights[i].color;

	}

	


	
}

