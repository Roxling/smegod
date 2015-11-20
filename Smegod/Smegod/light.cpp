#include "light.h"

Light::Light(shared_ptr<ShaderGroup> mshader_group)
{
	auto source = make_shared<Geometry>(ParametricShapes::createSphere(1.f, 10,10));
	source->world = glm::scale(source->world, glm::vec3(2.f));
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
