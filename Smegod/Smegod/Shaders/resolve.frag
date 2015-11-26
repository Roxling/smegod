#version 330 core

uniform sampler2D diffuse_buffer;
uniform sampler2D light_buffer;

out vec4 frag_color;

void main()
{
    const float gamma = 2.2;
    const vec3 ambient = vec3(0.0);

    vec4 diffuse = texelFetch(diffuse_buffer, ivec2(gl_FragCoord.xy), 0);
	if (diffuse.a < 0.5){
		frag_color = vec4(diffuse.rgb,1);
	}else{
		vec3 light   = texelFetch(light_buffer  , ivec2(gl_FragCoord.xy), 0).rgb;

		vec3 hdrColor = (light + ambient) * diffuse.rgb;
		float exposure = 0.8;
		// Exposure tone mapping
		vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
		// Gamma correction 
		mapped = pow(mapped, vec3(1.0 / gamma));
  
		frag_color = vec4(mapped, 1.0);
	}
		

    
}