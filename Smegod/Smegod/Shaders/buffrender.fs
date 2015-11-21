#version 330 core

in vec2 tex_coord;
out vec4 color;
uniform sampler2D buff;
uniform vec3 mask;

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
        float depth = text.r;
        ncolor.rgb = vec3(depth);
    }
    color = ncolor;
    color.a = 1;
} 