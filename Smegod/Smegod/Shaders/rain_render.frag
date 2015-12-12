#version 420 core
uniform sampler2DArray rainTextureArray;
layout (location = 0) out vec4 gRain;

in vec2 gs_tex;
flat in uint gs_type;
in float gs_random;

void main()
{
    vec4 color = texture(rainTextureArray, vec3(gs_tex,gs_type)) + texture(rainTextureArray, vec3(gs_tex,gs_random));
	color = normalize(color);
	color.a = 1;
	gRain =  color;
}

