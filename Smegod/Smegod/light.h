#pragma once
#include "smegod_dep.h"
#include "geometries.h"

class Light : public WorldObject, public ShaderCollection {
public:
	Light(shared_ptr<ShaderGroup>);
	void render(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader) override;
};


class SpotLight : public WorldObject {
public:
	SpotLight(glm::vec3 &color, float cutoff_deg = 12.5f, float outer_cutoff_deg = 17.5f, float linear_attenuation = 0.035f, float quadratic_attenuation = 0.044f, float sizescale = 1);

	void render(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader) override;
	
	glm::mat4 getLightSpaceMatrix();

private:
	void renderSelf();
	glm::mat4 m_light_projection;
	glm::vec3 m_color;
	float m_cutoff;
	float m_outer_cutoff;

	float m_linear_attenuation;
	float m_quadratic_attenuation;

	GLuint m_cone_vao = 0u;
	GLuint m_cone_vbo = 0u;
	GLsizei m_cone_vertices_nb = 0;
	void generate_cone(float outer_cutoff_deg, float sizescale);
};