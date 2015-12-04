#include "light.h"

static GLuint loadCone(GLuint& vboId, GLsizei& verticesNb, shared_ptr<ShaderGroup> shader)
{
	verticesNb = 65;
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

	GLuint vaoId = 0u;
	GLint loc = glGetAttribLocation(shader->getGlId(), "Vertex");
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);
	{
		glGenBuffers(1, &vboId);
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferData(GL_ARRAY_BUFFER, verticesNb * 3 * sizeof(float), vertexArrayData, GL_STATIC_DRAW);

		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(loc);
	}
	glBindVertexArray(0u);
	glBindBuffer(GL_ARRAY_BUFFER, 0u);

	return vaoId;
}

Light::Light(shared_ptr<ShaderGroup> mshader_group)
{
	auto source = make_shared<Geometry>(ParametricShapes::createSphere(1.f, 10,10));
	//source->world = glm::scale(source->world, glm::vec3(2.f));
	source->setColor(glm::vec3(1.f,0.6f,0));
	attach(source);
}

void Light::render(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader)
{
	GLuint light_pos;//for vs
	for (auto it = shader_groups.begin(); it != shader_groups.end(); ++it) {
		auto program = (*it)->getGlId();
		(*it)->use();

		light_pos = glGetUniformLocation(program, "light_pos");
		glUniform3fv(light_pos, 1, glm::value_ptr(combined_transform[3]));
	}
	
}

SpotLight::SpotLight(shared_ptr<ShaderGroup> shader)
{
	coneVao = loadCone(coneVbo, coneVerticesNb, shader);
	lightProjection = glm::perspective(45.f, (float)Globals::SHADOW_WIDTH / (float)Globals::SHADOW_HEIGHT, 0.1f, 10000.0f);
}

void SpotLight::render(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader)
{

	combined_world = combined_transform;
	auto wIT = glm::transpose(glm::inverse(combined_transform)); //is this the correct way to calculate the inverse transpose?

	shader->setUniform("world", combined_transform);
	shader->setUniform("light_color", LightColor);
	shader->setUniform("light_intensity", LightIntensity);
	shader->setUniform("light_anglefalloff", LightAngleFalloff);
	shader->setUniform("light_pos", glm::vec3(combined_transform[3]));
	shader->setUniform("light_dir", glm::vec3(combined_transform[2]));

	renderSelf();
}

glm::mat4 SpotLight::getLightSpaceMatrix()
{
	//lightProjection * lightOffsetTransform.GetMatrixInverse() * lightTransform.GetMatrixInverse();

	
	glm::mat4 lightView = glm::inverse(combined_world);
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	return lightSpaceMatrix;
}

void SpotLight::renderSelf()
{
	glBindVertexArray(coneVao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, coneVerticesNb);
	glBindVertexArray(0u);
}


