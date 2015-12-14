#include "light.h"

Light::Light(shared_ptr<ShaderGroup> mshader_group)
{
	auto source = make_shared<Geometry>(ParametricShapes::createSphere(1.f, 10,10));
	//source->world = glm::scale(source->world, glm::vec3(2.f));
	source->setColor(glm::vec3(1.f,0.6f,0));
	attach(source);
}

void Light::render(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader, bool renderMaterials)
{
	GLuint light_pos;//for vs
	for (auto it = shader_groups.begin(); it != shader_groups.end(); ++it) {
		auto program = (*it)->getGlId();
		(*it)->use();

		light_pos = glGetUniformLocation(program, "light_pos");
		glUniform3fv(light_pos, 1, glm::value_ptr(combined_transform[3]));
	}
}

SpotLight::SpotLight(glm::vec3 &color, float cutoff_deg, float outer_cutoff_deg, float linear_attenuation, float quadratic_attenuation, float sizescale) :
	m_color(color),
	m_cutoff(glm::cos(glm::radians(cutoff_deg))),
	m_outer_cutoff(glm::cos(glm::radians(outer_cutoff_deg))),
	m_linear_attenuation(linear_attenuation),
	m_quadratic_attenuation(quadratic_attenuation)
{
	m_light_projection = glm::perspective(45.f, (float)Globals::SHADOW_WIDTH / (float)Globals::SHADOW_HEIGHT, 0.1f, 500.0f);

	generate_cone(outer_cutoff_deg, sizescale);
}

void SpotLight::render(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader, bool renderMaterials)
{
	combined_world = combined_transform;

	shader->setUniform("u_world", combined_transform);

	bindUniform(shader, "u_light");
	renderSelf();
}

glm::mat4 SpotLight::getLightSpaceMatrix()
{
	glm::mat4 light_view = glm::inverse(combined_world);
	glm::mat4 light_space_matrix = m_light_projection * light_view;
	return light_space_matrix;
}

void SpotLight::bindUniform(shared_ptr<ShaderGroup> shader, string prefix)
{
	shader->setUniform(prefix + ".position", glm::vec3(combined_world[3]));
	shader->setUniform(prefix + ".direction", glm::vec3(combined_world[2]));
	shader->setUniform(prefix + ".color", m_color);

	shader->setUniform(prefix + ".cutOff", m_cutoff);
	shader->setUniform(prefix + ".outerCutOff", m_outer_cutoff);

	shader->setUniform(prefix + ".linear", m_linear_attenuation);
	shader->setUniform(prefix + ".quadratic", m_quadratic_attenuation);
}

void SpotLight::renderSelf()
{
	glBindVertexArray(m_cone_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_cone_vertices_nb);
	glBindVertexArray(0u);
}

void SpotLight::generate_cone(float outer_cutoff_deg, float sizescale)
{
	m_cone_vertices_nb = 65;
	float vertexArrayData[65 * 3] = {
		0.f, 1.f, -1.f,
		0.f, 0.f, 0.f,
		0.38268f, 0.92388f, -1.f,
		0.f, 0.f, 0.f,
		0.70711f, 0.70711f, -1.f,
		0.f, 0.f, 0.f,
		0.92388f, 0.38268f, -1.f,
		0.f, 0.f, 0.f,
		1.f, 0.f, -1.f,
		0.f, 0.f, 0.f,
		0.92388f, -0.38268f, -1.f,
		0.f, 0.f, 0.f,
		0.70711f, -0.70711f, -1.f,
		0.f, 0.f, 0.f,
		0.38268f, -0.92388f, -1.f,
		0.f, 0.f, 0.f,
		0.f, -1.f, -1.f,
		0.f, 0.f, 0.f,
		-0.38268f, -0.92388f, -1.f,
		0.f, 0.f, 0.f,
		-0.70711f, -0.70711f, -1.f,
		0.f, 0.f, 0.f,
		-0.92388f, -0.38268f, -1.f,
		0.f, 0.f, 0.f,
		-1.f, 0.f, -1.f,
		0.f, 0.f, 0.f,
		-0.92388f, 0.38268f, -1.f,
		0.f, 0.f, 0.f,
		-0.70711f, 0.70711f, -1.f,
		0.f, 0.f, 0.f,
		-0.38268f, 0.92388f, -1.f,
		0.f, 1.f, -1.f,
		0.f, 1.f, -1.f,
		0.38268f, 0.92388f, -1.f,
		0.f, 1.f, -1.f,
		0.70711f, 0.70711f, -1.f,
		0.f, 0.f, -1.f,
		0.92388f, 0.38268f, -1.f,
		0.f, 0.f, -1.f,
		1.f, 0.f, -1.f,
		0.f, 0.f, -1.f,
		0.92388f, -0.38268f, -1.f,
		0.f, 0.f, -1.f,
		0.70711f, -0.70711f, -1.f,
		0.f, 0.f, -1.f,
		0.38268f, -0.92388f, -1.f,
		0.f, 0.f, -1.f,
		0.f, -1.f, -1.f,
		0.f, 0.f, -1.f,
		-0.38268f, -0.92388f, -1.f,
		0.f, 0.f, -1.f,
		-0.70711f, -0.70711f, -1.f,
		0.f, 0.f, -1.f,
		-0.92388f, -0.38268f, -1.f,
		0.f, 0.f, -1.f,
		-1.f, 0.f, -1.f,
		0.f, 0.f, -1.f,
		-0.92388f, 0.38268f, -1.f,
		0.f, 0.f, -1.f,
		-0.70711f, 0.70711f, -1.f,
		0.f, 0.f, -1.f,
		-0.38268f, 0.92388f, -1.f,
		0.f, 0.f, -1.f,
		0.f, 1.f, -1.f,
		0.f, 0.f, -1.f
	};

	// Scale to fit light volume... DRAGONS!
	GLfloat max_component = std::fmaxf(std::fmaxf(m_color.r, m_color.g), m_color.b);
	GLfloat max_cone_height = sizescale * (-m_linear_attenuation + std::sqrtf(m_linear_attenuation * m_linear_attenuation - 4 * m_quadratic_attenuation * (1.0f - (256.0f / 5.0f) * max_component))) / (2 * m_quadratic_attenuation);
	GLfloat base_radius = max_cone_height * std::tan(glm::radians(outer_cutoff_deg));

	for (int i = 0; i < 65; i++) {
		vertexArrayData[i * 3] *= base_radius;
		vertexArrayData[i * 3 + 1] *= base_radius;
		vertexArrayData[i * 3 + 2] *= max_cone_height;
	}

	glGenVertexArrays(1, &m_cone_vao);
	glBindVertexArray(m_cone_vao);
	{
		glGenBuffers(1, &m_cone_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_cone_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_cone_vertices_nb * 3 * sizeof(float), vertexArrayData, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
	}
	glBindVertexArray(0u);
	glBindBuffer(GL_ARRAY_BUFFER, 0u);
}


