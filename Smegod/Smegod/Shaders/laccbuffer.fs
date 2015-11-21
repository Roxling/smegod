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

layout (location = 3) out vec4 light_contribution;

#define EXTRACT_DEPTH(cc)	((cc).b + (cc).g / 256.0 + (cc).r / (256.0 * 256.0) + (cc).a / (256.0 * 256.0 * 256.0))

void main()
{
    mat4 ViewProjection = projection * view;
    mat4 ViewProjectionInverse = inverse(ViewProjection);

    vec2 tex_coord = gl_FragCoord.xy * invRes;

    vec4 depth_tex = texture(depthBuffer, tex_coord);
    float depth = EXTRACT_DEPTH(depth_tex);

    vec4 pixel_pos = vec4(tex_coord.x, tex_coord.y, depth, 1);
    vec3 pixel_world = (ViewProjectionInverse * pixel_pos).xyz;

    vec4 NnS = texture(normalAndSpecularBuffer, tex_coord);
    vec3 N = NnS.xyz*2 - 1;
    float shininess = NnS.z;

    N = normalize(N);
    vec3 L = normalize(light_dir);
    vec3 V = normalize(camera_pos - pixel_world);
    vec3 R = normalize(reflect(-L,N));
    vec3 diffuse = light_color * max(dot(L,N),0.0);
    vec3 specular = light_color * pow(max(dot(V,R),0.0), shininess);
    light_contribution.xyz = diffuse + specular; 
    light_contribution.w = 1;
}