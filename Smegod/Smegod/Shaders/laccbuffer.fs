#version 330 core

uniform sampler2D depthBuffer;
uniform sampler2D normalAndSpecularBuffer;
uniform sampler2DShadow shadowMap;

uniform vec2 invRes;


uniform mat4 view;
uniform mat4 projection;

uniform vec3 camera_pos;
uniform mat4 shadowViewProjection;

uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 light_dir;
uniform float light_intensity;
uniform float light_anglefalloff;

uniform vec2 shadow_texelsize;

layout (location = 0) out vec4 light_contribution;

#define EXTRACT_DEPTH(cc)	((cc).b + (cc).g / 256.0 + (cc).r / (256.0 * 256.0) + (cc).a / (256.0 * 256.0 * 256.0))

void main()
{
    mat4 ViewProjection = projection * view;
    mat4 ViewProjectionInverse = inverse(ViewProjection);

    vec2 screen_coord; 
    screen_coord.x = gl_FragCoord.x / 1500;
    screen_coord.y = gl_FragCoord.y / 720;

    vec2 tex_coord = gl_FragCoord.xy * invRes;



    vec4 depth_tex = texture(depthBuffer, tex_coord);
    float depth = EXTRACT_DEPTH(depth_tex);


    //Composite light using phong shading, falloffs and LightIntensity and LightColor

    vec4 pixel_pos = vec4(tex_coord.x*2 - 1, tex_coord.y*2 - 1, depth, 1);
    vec4 pixel_world = (ViewProjectionInverse * pixel_pos);

    pixel_world.x = pixel_world.x/pixel_world.w;
    pixel_world.y = pixel_world.y/pixel_world.w;
    pixel_world.z = pixel_world.z/pixel_world.w;
    pixel_world.w = 1;

    float length = distance(light_pos, pixel_world.xyz);
    vec3 color = light_color/(pow(length*2, 2));

  


    vec4 NnS = texture(normalAndSpecularBuffer, tex_coord);
    vec3 N = NnS.xyz*2 - 1;
    float shininess = NnS.z*100;



    N = normalize(N);
    vec3 L = normalize(light_pos - pixel_world.xyz);
    float rad = acos(dot(L, normalize(light_dir)));
    if(rad > light_anglefalloff)
        rad = light_anglefalloff;

    float radialFalloff = 1 - rad/light_anglefalloff;
    
   
    vec3 V = normalize(camera_pos - pixel_world.xyz);

    vec3 R = normalize(reflect(-L,N));
    vec3 diffuse = color * max(dot(L,N),0.0);
    vec3 specular = color * pow(max(dot(V,R),0.0), shininess);

    

    light_contribution.xyz = vec3(gl_FragCoord.x/1600);
    light_contribution.w = 1;
}