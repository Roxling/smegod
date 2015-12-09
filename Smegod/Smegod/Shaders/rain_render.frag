#version 330 core
uniform sampler2DArray rainTextureArray;
layout (location = 4) out vec4 gRain;
in vec2 gs_tex;
flat in uint gs_type;
//out vec4 frag_color;

void main()
{
    vec4 color =texture(rainTextureArray, vec3(gs_tex,1));

	color.a = clamp(0,1,pow(color.r,0.5));
	gRain = color;
}

