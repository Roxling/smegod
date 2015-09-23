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
	glm::vec3 ambient(0.1f,0,0);
	glm::vec3 diffuse(0.2f);
	glm::vec3 specular(0.2f);
	GLfloat shininess = 80.f;
	
	GLuint ambient_loc, diffuse_loc, specular_loc, shininess_loc; //for fs
	GLuint light_pos;//for vs
	for (auto it = shader_groups.begin(); it != shader_groups.end(); ++it) {
		auto program = (*it)->getProgram();
		(*it)->use();
		ambient_loc = glGetUniformLocation(program, "kambient");
		glUniform3fv(ambient_loc, 1, glm::value_ptr(ambient));
		diffuse_loc = glGetUniformLocation(program, "kdiffuse");
		glUniform3fv(diffuse_loc, 1, glm::value_ptr(diffuse));
		specular_loc = glGetUniformLocation(program, "kspecular");
		glUniform3fv(specular_loc, 1, glm::value_ptr(specular));
		shininess_loc = glGetUniformLocation(program, "shininess");
		glUniform1fv(shininess_loc, 1,(const float *) &shininess);

		light_pos = glGetUniformLocation(program, "light_pos");
		glUniform3fv(light_pos, 1, glm::value_ptr(combined_transform[3]));
	}
	
}
