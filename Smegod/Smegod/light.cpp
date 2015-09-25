#include "light.h"

Light::Light(shared_ptr<ShaderGroup> mshader_group) : Geometry(mshader_group)
{
	shared_ptr<Cube> source = make_shared<Cube>(mshader_group);
	source->world = glm::scale(source->world, glm::vec3(1.f));
	source->color = glm::vec3(1.f,0,0);
	attach(source);
}

void Light::renderSelf(glm::mat4 combined_transform)
{
	GLuint light_pos;//for vs
	for (auto it = shader_groups.begin(); it != shader_groups.end(); ++it) {
		auto program = (*it)->getProgram();
		(*it)->use();

		light_pos = glGetUniformLocation(program, "light_pos");
		glUniform3fv(light_pos, 1, glm::value_ptr(combined_transform[3]));
	}
	
}
