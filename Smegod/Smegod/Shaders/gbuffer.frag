#version 420 core
layout (location = 0) out vec4 geometry_diffuse;
layout (location = 1) out vec4 geometry_normal_and_specular;
layout (location = 2) out vec4 bloom_filter;
layout (location = 3) out vec4 light;

in vec4 worldspace_position;
in vec3 worldspace_normal;
in vec3 worldspace_binormal;
in vec3 worldspace_tangent;
in vec2 pass_texcoords;

in vec3 tangent_view_position;
in vec3 tangent_frag_position;

uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;
uniform sampler2D normal_texture;
uniform sampler2D displacement_texture;

uniform sampler3D splash_bump_texture;
uniform sampler3D splash_texture;

const float ambient = 0.08;

uniform float u_KsDir;
uniform float u_timeCycle;
uniform vec2 u_splash_offset;

#define saturate(a) clamp( a, 0.0, 1.0 )

uniform float u_height_scale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    float height =  texture(displacement_texture, texCoords).r;     
    return texCoords - viewDir.xy / viewDir.z * (height * u_height_scale); 
} 

void main()
{
    // Offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(tangent_view_position - tangent_frag_position);
    vec2 texCoords = ParallaxMapping(pass_texcoords,  viewDir);


	vec3 tangent = normalize(worldspace_tangent);
	vec3 binormal = normalize(worldspace_binormal);
	vec3 normal = normalize(worldspace_normal);
	vec4 position = worldspace_position / worldspace_position.w;

	// Worldspace normal
	//geometry_normal_and_specular.xyz = texture(normal_texture, pass_texcoords).rgb;
    vec3 bump = texture(normal_texture, pass_texcoords).rgb;
	bump = bump * 2 - 1;
	vec3 N =  tangent * bump.x + binormal * bump.y + normal * bump.z;
    N = normalize(N);

	//Spash mapping
	//Filter = ANISOTROPIC;
	//AddressU = Mirror;
	//AddressV = Mirror;
	float wetSurf = saturate(u_KsDir/2.0*saturate(normal.y));
	
	vec3 splash_coord = vec3(position.x/2.0 + u_splash_offset.x, position.z/2.0 + u_splash_offset.y, u_timeCycle);
	vec4 spash_bump = texture(splash_bump_texture, splash_coord) - 0.5;
    N += wetSurf * 3 * (spash_bump.x * tangent + spash_bump.y * binormal);
	N = normalize(N);
    float splashDiffuse = wetSurf * texture(splash_texture, splash_coord).r;


	geometry_normal_and_specular.xyz = (N*0.5 + 0.5);
	// Specularity
	geometry_normal_and_specular.a = texture(specular_texture, pass_texcoords).r + splashDiffuse;

	// Diffuse color
	geometry_diffuse = texture(diffuse_texture, pass_texcoords);
	if (geometry_diffuse.a < 0.2) discard;
    if (geometry_diffuse.a < 0.5) {
        bloom_filter = vec4(20, 20, 2, 1);
    }
	geometry_diffuse.a = ambient;
}