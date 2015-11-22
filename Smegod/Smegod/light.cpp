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
	GLint loc = glGetAttribLocation(shader->getProgram(), "Vertex");
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
		auto program = (*it)->getProgram();
		(*it)->use();

		light_pos = glGetUniformLocation(program, "light_pos");
		glUniform3fv(light_pos, 1, glm::value_ptr(combined_transform[3]));
	}
	
}

SpotLight::SpotLight(shared_ptr<ShaderGroup> shader)
{
	world = glm::scale(world, glm::vec3(10));
	coneVao = loadCone(coneVbo, coneVerticesNb, shader);
}

void SpotLight::render(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader)
{
	auto program = shader->getProgram();
	auto wIT = glm::transpose(glm::inverse(combined_transform)); //is this the correct way to calculate the inverse transpose?
	world_location = glGetUniformLocation(shader->getProgram(), "world");
	worldIT_location = glGetUniformLocation(shader->getProgram(), "worldIT");
	glUniformMatrix4fv(world_location, 1, GL_FALSE, glm::value_ptr(combined_transform));
	glUniformMatrix4fv(worldIT_location, 1, GL_FALSE, glm::value_ptr(wIT));

	GLint color = glGetUniformLocation(shader->getProgram(), "light_color");
	glUniform3fv(color, 1, glm::value_ptr(LightColor));

	GLint intensity = glGetUniformLocation(shader->getProgram(), "light_intensity");
	glUniform1fv(intensity, 1, (const float *)&LightIntensity);

	GLint falloff = glGetUniformLocation(shader->getProgram(), "light_anglefalloff");
	glUniform1fv(falloff, 1, (const float *)&LightAngleFalloff);

	GLint texel = glGetUniformLocation(shader->getProgram(), "shadow_texelsize");
	glUniform2fv(texel, 1, glm::value_ptr(ShadowMapTexelSize));

	GLint position = glGetUniformLocation(shader->getProgram(), "light_pos");
	glUniform3fv(position, 1, glm::value_ptr(combined_transform[3]));

	GLint dir = glGetUniformLocation(shader->getProgram(), "light_dir");
	glUniform3fv(dir, 1, glm::value_ptr(combined_transform[2]));

	GLint inv = glGetUniformLocation(shader->getProgram(), "invRes");
	glUniform2fv(inv, 1, glm::value_ptr(invRes));

	renderSelf();
}

glm::mat4 SpotLight::getLightSpaceMatrix()
{
	//lightProjection * lightOffsetTransform.GetMatrixInverse() * lightTransform.GetMatrixInverse();

	glm::mat4 lightProjection = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);
	glm::mat4 lightView = glm::inverse(world);
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	return lightSpaceMatrix;
}

void SpotLight::renderSelf()
{
	glBindVertexArray(coneVao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, coneVerticesNb);
	glBindVertexArray(0u);
}


