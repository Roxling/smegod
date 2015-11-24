#version 330 core

uniform sampler2D diffuse_buffer;
uniform sampler2D light_buffer;

out vec4 frag_color;

void main()
{
    vec4 diffuse = texelFetch(diffuse_buffer, ivec2(gl_FragCoord.xy), 0);
	if (diffuse.a < 0.2)
		discard;

    vec3 light   = texelFetch(light_buffer  , ivec2(gl_FragCoord.xy), 0).rgb;
    const vec3 ambient = vec3(0.1);

	frag_color = vec4((light + ambient) * diffuse.rgb, 1.0);
}
