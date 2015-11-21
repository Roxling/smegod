#version 330 core
layout (location = 0) out vec4 geometry_diffuse;
layout (location = 1) out vec4 geometry_normal_and_specular;

in vec3 worldspace_normal;
in vec3 worldspace_binormal;
in vec3 worldspace_tangent;
in vec2 pass_texcoords;

uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;
uniform sampler2D normal_texture;


void main()
{
	// Diffuse color
	geometry_diffuse = texture(diffuse_texture, pass_texcoords);
	if (geometry_diffuse.a < 0.2) discard;

	// Worldspace normal
	//geometry_normal_and_specular.xyz = texture(normal_texture, pass_texcoords).rgb;
    vec3 bump = texture(normal_texture, pass_texcoords).rgb;
	bump = bump * 2 - 1;

	vec3 N =  worldspace_tangent * bump.x + worldspace_binormal * bump.y + worldspace_normal * bump.z;

    N = normalize(N);

	geometry_normal_and_specular.xyz = (N*0.5 + 0.5);
	// Specularity
    vec4 spec = texture(specular_texture, pass_texcoords);
    float mspec = (spec.r+spec.g+spec.b)/3;
	geometry_normal_and_specular.a = mspec;
}