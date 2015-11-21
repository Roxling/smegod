#version 330 core
layout (location = 0) out vec4 geometry_diffuse;
layout (location = 1) out vec4 geometry_normal_and_specular;
layout (location = 2) out vec4 geometry_depth;

in vec3 worldspace_normal;
in vec3 worldspace_binormal;
in vec3 worldspace_tangent;
in vec2 pass_texcoords;

uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;
uniform sampler2D normal_texture;

vec4 encodeDepth(float depth)
{
	float d0 = depth;
	float d1 = depth * 256.0;
	float d2 = depth * 256.0 * 256.0;
	float d3 = depth * 256.0 * 256.0 * 256.0;

	vec4 result;

	result.a = mod(d3, 1.0);
	result.r = mod((d2 - result.a / 256.0), 1.0);
	result.g = mod((d1 - result.r / 256.0), 1.0);
	result.b = d0 - result.g / 256.0;
	return result;
}

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
    geometry_depth = encodeDepth(gl_FragCoord.z);

}