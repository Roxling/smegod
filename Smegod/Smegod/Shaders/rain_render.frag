#version 330 core
uniform sampler2DArray rainTextureArray;

in vec2 gs_tex;
flat in uint gs_type;
out vec4 frag_color;

void main()
{
    vec4 color =texture(rainTextureArray, vec3(gs_tex,int(gs_type)*2));

	color.a = clamp(0,1,pow(color.r,0.5));
	frag_color = color;
}

