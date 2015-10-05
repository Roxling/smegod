#include "geometries.h"

void Geometry::render(glm::mat4 combined_transform)
{
	shader_group->use();
	auto program = shader_group->getProgram();
	auto wIT = glm::transpose(glm::inverse(combined_transform)); //is this the correct way to calculate the inverse transpose?
	world_location = glGetUniformLocation(shader_group->getProgram(), "world");
	worldIT_location = glGetUniformLocation(shader_group->getProgram(), "worldIT");
	glUniformMatrix4fv(world_location, 1, GL_FALSE, glm::value_ptr(combined_transform));
	glUniformMatrix4fv(worldIT_location, 1, GL_FALSE, glm::value_ptr(wIT));
	//color
	GLuint color_location = glGetUniformLocation(shader_group->getProgram(), "incolor");
	glUniform3fv(color_location, 1, glm::value_ptr(color));
	//material
	GLuint ambient_loc, diffuse_loc, specular_loc, shininess_loc; //for fs
	ambient_loc = glGetUniformLocation(program, "kambient");
	glUniform3fv(ambient_loc, 1, glm::value_ptr(material.ambient));
	diffuse_loc = glGetUniformLocation(program, "kdiffuse");
	glUniform3fv(diffuse_loc, 1, glm::value_ptr(material.diffuse));
	specular_loc = glGetUniformLocation(program, "kspecular");
	glUniform3fv(specular_loc, 1, glm::value_ptr(material.specular));
	shininess_loc = glGetUniformLocation(program, "shininess");
	glUniform1fv(shininess_loc, 1, (const float *)&material.shininess);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->texture_id);
	glUniform1i(glGetUniformLocation(shader_group->getProgram(), "tex"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bumpmap->texture_id);
	glUniform1i(glGetUniformLocation(shader_group->getProgram(), "bump"), 1);
	renderSelf();
}

Geometry::Geometry(shared_ptr<ShaderGroup> mshader_group, VertexArray va) : WorldObject(mshader_group)
{
	vertex_array = make_unique<VertexArray>(va);
}

void Geometry::renderSelf()
{
	//glBindTexture(GL_TEXTURE_2D, texture->texture_id);
	glBindVertexArray(vertex_array->VAO);
	glDrawElements(GL_TRIANGLES, vertex_array->num_indices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


Frame::Frame(shared_ptr<ShaderGroup> mshader_group) : WorldObject(mshader_group)
{
	//X
	auto axis = make_shared<Geometry>(mshader_group,ParametricShapes::createCube(1.f,1));
	axis->world = glm::scale(axis->world, glm::vec3(2.f,0.1f,0.1f));
	axis->translate(.05f, 0, 0);
	axis->color = { 1.f, 0, 0 };
	attach(axis);
	//Y
	axis = make_shared<Geometry>(mshader_group, ParametricShapes::createCube(1.f, 1));
	axis->world = glm::scale(axis->world, glm::vec3(0.1f, 2.f, 0.1f));
	axis->translate(0, .05f, 0);
	axis->color = { 0, 1.f, 0 };
	attach(axis);
	//Z
	axis = make_shared<Geometry>(mshader_group, ParametricShapes::createCube(1.f, 1));
	axis->world = glm::scale(axis->world, glm::vec3(0.1f, 0.1f, 2.f));
	axis->translate(0, 0, 1.0f);
	axis->color = { 0, 0, 1.f};
	attach(axis);

	//box
	axis = make_shared<Geometry>(mshader_group, ParametricShapes::createCube(1.f, 1));
	axis->world = glm::scale(axis->world, glm::vec3(.1f));
	axis->color = glm::vec3{ 0.f };
	attach(axis);
}


Skybox::Skybox(shared_ptr<Cubemap> cubemap) : WorldObject(make_shared<ShaderGroup>("cubemap.vs", "cubemap.fs")), cubemap(cubemap) {
	skybox = make_shared<VertexArray>(ParametricShapes::createSphere(20, 10, 10, true));
}

void Skybox::render(glm::mat4 combined_transform) {
	glDepthMask(GL_FALSE);
	shader_group->use();

	glBindVertexArray(skybox->VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->texture_id);
	glDrawElements(GL_TRIANGLES, skybox->num_indices, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	glDepthMask(GL_TRUE);
}