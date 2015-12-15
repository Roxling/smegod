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
#define NUM_LIGHTS (5)
uniform Light lights[NUM_LIGHTS];

in vec2 gs_tex;
in vec3 gs_pos;
in float gs_type;
in float gs_random;


#define AMBIENT (0.8)
void main()
{

    vec4 texture = texture(rainTextureArray, vec3(gs_tex,gs_type));

	vec4 light = vec4(0);
	for(int i = 0; i < NUM_LIGHTS; i++){
		vec3 rD = normalize( lights[i].position - gs_pos );

		float theta = dot(rD, normalize(lights[i].direction)); 
		float epsilon = (lights[i].cutOff - lights[i].outerCutOff);
		float intensity = saturate((theta - lights[i].outerCutOff) / epsilon);

		float la = lights[i].linear;
		float qa = lights[i].quadratic;
		float dist =  distance(lights[i].position, gs_pos);
		float attenuation = 1.0 / (1.0 + la * dist + qa * (dist * dist));

		light += intensity * attenuation * vec4(lights[i].color,1) * texture;

	}
	gRain = light + AMBIENT * texture * gs_random;

	
}

