#version 330 core

in vec2 tex_coord;
out vec4 color;
uniform sampler2D buff;
uniform vec3 mask;

#define EXTRACT_DEPTH(cc)	((cc).b + (cc).g / 256.0 + (cc).r / (256.0 * 256.0) + (cc).a / (256.0 * 256.0 * 256.0))

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
        float depth = EXTRACT_DEPTH(text);
        ncolor.rgb = vec3(depth);
    }
    color = ncolor;
    color.a = 1;
} 