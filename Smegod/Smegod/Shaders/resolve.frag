#version 330 core

uniform sampler2D diffuse_buffer;
uniform sampler2D light_buffer;
uniform sampler2D bloom_buffer;
uniform sampler2D rain_buffer;

uniform vec2 invRes;

out vec4 frag_color;

void main()
{
    vec2 screen_coord = gl_FragCoord.xy * invRes; // [0,1]
    vec2 ndc = screen_coord * 2 - 1; // [-1,1]

    const float gamma = 1;

    vec4 diffuse = texelFetch(diffuse_buffer, ivec2(gl_FragCoord.xy), 0);
	diffuse.rgb *= diffuse.a;
	diffuse.a = 1;

    vec3 light = texelFetch(light_buffer, ivec2(gl_FragCoord.xy), 0).rgb;
    vec3 bloom = texture(bloom_buffer, screen_coord).rgb;
	vec3 rain = texelFetch(rain_buffer, ivec2(gl_FragCoord.xy), 0).rgb;

    vec3 hdrColor = (light + bloom) + diffuse.rgb + rain;
    float exposure =1;
    // Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // Gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    frag_color = vec4(mapped, 1.0);
}
