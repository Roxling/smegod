#include "light.h"

Light::Light(shared_ptr<ShaderGroup> mshader_group) : Geometry(mshader_group)
{
	shared_ptr<Cube> source = make_shared<Cube>(mshader_group);
	source->world = glm::scale(source->world, glm::vec3(0.2f));
	source->color = glm::vec3(1.f);
	attach(source);
}

void Light::render()
{
	glm::vec3 ambient(0.2f);
	glm::vec3 diffuse(0.2f);
	glm::vec3 specular(0.2f);
	/*
	GLuint ambient_loc, diffuse_loc, specular_loc;
	for (auto it = shader_groups.begin(); it != shader_groups.end(); ++it) {
		auto program = (*it)->getProgram();
		(*it)->use();
		ambient_loc = glGetUniformLocation(program, "ambient");
		glUniform3fv(ambient_loc, 1, glm::value_ptr(ambient));
		diffuse_loc = glGetUniformLocation(program, "diffuse");
		glUniform3fv(diffuse_loc, 1, glm::value_ptr(diffuse));
		specular_loc = glGetUniformLocation(program, "specular");
		glUniform3fv(specular_loc, 1, glm::value_ptr(specular));
	}
	*/
}
