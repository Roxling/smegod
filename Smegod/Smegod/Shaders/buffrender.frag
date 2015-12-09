#version 330 core

in vec2 tex_coord;
out vec4 color;
uniform sampler2D buff;
uniform vec3 mask;


uniform float near; 
uniform float far;
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
	vec4 text = texture(buff, tex_coord);
	vec4 ncolor = vec4(1);
	if(mask.x == 1){
		ncolor.rgb = text.rgb;
	}else{
			ncolor.rgb = vec3(text.a);
	}
	if(mask.y == 1){
		float linearDepth = LinearizeDepth(text.r);
		ncolor.rgb = vec3(linearDepth);
	}
	color = ncolor;
	color.a = 1;
} 