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

uniform float u_KsDir;
uniform float u_timeCycle;
uniform vec2 u_splash_offset;
uniform vec3 u_camera_pos;
uniform float u_height_scale;
 
#define AMBIENT (0.08);
#define MIN_LAYERS (20)
#define MAX_LAYERS (40)
#define MAX_ITER (100)

#define saturate(a) clamp( a, 0.0, 1.0 )

vec2 ParallaxMappingOcc(vec3 view_dir)
{ 
    float num_layers = mix(MAX_LAYERS, MIN_LAYERS, abs(dot(vec3(0.0, 0.0, 1.0), view_dir)));  
    float layer_depth = 1.0 / num_layers;
    float current_layer_depth = 0.0;

    vec2 P = view_dir.xy / view_dir.z * u_height_scale; 
    vec2 delta_texcoords = P / num_layers;
  
    vec2 current_texcoords = pass_texcoords - floor(pass_texcoords);
    float current_depth = texture(displacement_texture, current_texcoords).r;
    
	int i = 0;
    while(current_layer_depth < current_depth && i++ < MAX_ITER )
    {
        current_texcoords -= delta_texcoords;
        current_depth = texture(displacement_texture, current_texcoords).r;  
        current_layer_depth += layer_depth;  
    }
    
    vec2 prev_texcoords = current_texcoords + delta_texcoords;

    float after_depth  = current_depth - current_layer_depth;
    float before_depth = texture(displacement_texture, prev_texcoords).r - current_layer_depth + layer_depth;
 
    float weight = after_depth / (after_depth - before_depth);
    return prev_texcoords * weight + current_texcoords * (1.0 - weight);
}

void main()
{
	vec3 tangent = normalize(worldspace_tangent);
	vec3 binormal = normalize(worldspace_binormal);
	vec3 normal = normalize(worldspace_normal);
	vec4 position = worldspace_position / worldspace_position.w;

    vec3 viewDir = normalize(tangent_view_position - tangent_frag_position);
    vec2 texCoords = ParallaxMappingOcc( viewDir);
	
	// Bump mapping
    vec3 bump = texture(normal_texture, texCoords).rgb;
	bump = bump * 2 - 1;
	vec3 N = tangent * bump.x + binormal * bump.y + normal * bump.z;
    N = normalize(N);

	//Spash mapping
	float wetSurf = saturate(u_KsDir/2.0*saturate(normal.y));
	
	vec3 splash_coord = vec3(position.x/2.0 + u_splash_offset.x, position.z/2.0 + u_splash_offset.y, u_timeCycle);
	vec4 splash_bump = texture(splash_bump_texture, splash_coord) - 0.5;
    N += wetSurf * 3 * (splash_bump.x * tangent + splash_bump.y * binormal);
	N = normalize(N);
    float splashDiffuse = wetSurf * texture(splash_texture, splash_coord).r;

	geometry_normal_and_specular.xyz = (N*0.5 + 0.5);

	// Specularity
	geometry_normal_and_specular.a = texture(specular_texture, texCoords).r + splashDiffuse;

	// Diffuse color
	geometry_diffuse = texture(diffuse_texture, texCoords);
	
	// Hax to add lights to bloom filter
	if (geometry_diffuse.a < 0.5) {
        bloom_filter = vec4(geometry_diffuse.rgb*20,1);
    }

	geometry_diffuse.a = AMBIENT;
}