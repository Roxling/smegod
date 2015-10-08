#include "light.h"

Light::Light(shared_ptr<ShaderGroup> mshader_group) : WorldObject(mshader_group)
{
	auto source = make_shared<Geometry>(mshader_group, ParametricShapes::createSphere(1.f, 10,10));
	source->world = glm::scale(source->world, glm::vec3(20.f));
	source->color = glm::vec3(1.f,0.6f,0);
	attach(source);
}

void Light::render(glm::mat4 combined_transform)
{
	GLuint light_pos;//for vs
	for (auto it = shader_groups.begin(); it != shader_groups.end(); ++it) {
		auto program = (*it)->getProgram();
		(*it)->use();

		light_pos = glGetUniformLocation(program, "light_pos");
		glUniform3fv(light_pos, 1, glm::value_ptr(combined_transform[3]));
	}
	
}
