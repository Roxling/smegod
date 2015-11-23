#include "geometries.h"

void Geometry::render(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader)
{
	auto wIT = glm::transpose(glm::inverse(combined_transform)); //is this the correct way to calculate the inverse transpose?
	shader->setUniform("world", combined_transform);
	shader->setUniform("worldIT", wIT);

	for (auto it = model->meshes.begin(); it != model->meshes.end(); ++it){
		shader->bindMaterial(it->material);
		renderSelf(*it);
	}
}

void Geometry::setColor(glm::vec3 rgb)
{
	for (auto it = model->meshes.begin(); it != model->meshes.end(); ++it) {
		it->material->color = rgb;
	}
}

void Geometry::bindTexture(string glslName, GLuint id)
{
	for (auto it = model->meshes.begin(); it != model->meshes.end(); ++it) {
		it->material->textures.push_back({glslName,id});
	}
}

Geometry::Geometry(VertexArray va) : WorldObject()
{
	model = make_unique<Model>(va);
}

Geometry::Geometry(Model m) : WorldObject()
{
	model = make_unique<Model>(m);
}

void Geometry::renderSelf(Mesh &mesh)
{	
	glBindVertexArray(mesh.va.VAO);
	glDrawElements(GL_TRIANGLES, mesh.va.num_indices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


Frame::Frame(shared_ptr<ShaderGroup> mshader_group)
{
	//X
	auto axis = make_shared<Geometry>(ParametricShapes::createCube(1.f,1));
	axis->world = glm::scale(axis->world, glm::vec3(2.f,0.1f,0.1f));
	axis->translate(.05f, 0, 0);
	axis->setColor({ 1.f, 0, 0 });
	attach(axis);
	//Y
	axis = make_shared<Geometry>(ParametricShapes::createCube(1.f, 1));
	axis->world = glm::scale(axis->world, glm::vec3(0.1f, 2.f, 0.1f));
	axis->translate(0, .05f, 0);
	axis->setColor({ 0, 1.f, 0 });
	attach(axis);
	//Z
	axis = make_shared<Geometry>(ParametricShapes::createCube(1.f, 1));
	axis->world = glm::scale(axis->world, glm::vec3(0.1f, 0.1f, 2.f));
	axis->translate(0, 0, 1.0f);
	axis->setColor({ 0, 0, 1.f});
	attach(axis);

	//box
	axis = make_shared<Geometry>(ParametricShapes::createCube(1.f, 1));
	axis->world = glm::scale(axis->world, glm::vec3(.1f));
	axis->setColor(glm::vec3{ 0.f });
	attach(axis);
}


Skybox::Skybox(shared_ptr<Cubemap> cubemap) : cubemap(cubemap) {
	skybox = make_shared<VertexArray>(ParametricShapes::createSphere(200, 4, 4, true));
}

void Skybox::render(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader) {
	glDepthMask(GL_FALSE);
	shader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(skybox->VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->texture_id);
	glDrawElements(GL_TRIANGLES, skybox->num_indices, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	glDepthMask(GL_TRUE);
}