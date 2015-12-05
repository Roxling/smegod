#include "geometries.h"

void Geometry::render(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader)
{
	shader->setUniform("world", combined_transform);
	GL_CHECK_ERRORS_MSG("Geometry bind World matrix");
	for (auto it = model->meshes.begin(); it != model->meshes.end(); ++it){
		shader->bindMaterial(it->material);
		GL_CHECK_ERRORS_MSG("Geometry bind material");
		renderSelf(*it);
		GL_CHECK_ERRORS_MSG("Geometry render self");
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
	GL_CHECK_ERRORS_MSG("Geometry bind VAO");
	glDrawElements(GL_TRIANGLES, mesh.va.num_indices, GL_UNSIGNED_INT, 0);
	GL_CHECK_ERRORS_MSG("Geometry glDrawElements");
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

Quad::Quad() : Quad(-1.f, -1.f, 1.f, 1.f) { }
Quad::Quad(glm::vec2 &lb, glm::vec2 &tr) : Quad(lb.x, lb.y, tr.x, tr.y) { }

Quad::Quad(float x1, float y1, float x2, float y2)
{
	GLfloat quadVertices[] = {
		// Positions        // Texture Coords
		x1, y2, 0.0f, 0.0f, 1.0f,
		x1, y1, 0.0f, 0.0f, 0.0f,
		x2, y2, 0.0f, 1.0f, 1.0f,
		x2, y1, 0.0f, 1.0f, 0.0f,
	};
	// Setup plane VAO
	glGenVertexArrays(1, &quadVAO);
	GL_CHECK_ERRORS();
	glGenBuffers(1, &quadVBO);
	GL_CHECK_ERRORS();
	glBindVertexArray(quadVAO);
	GL_CHECK_ERRORS();
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	GL_CHECK_ERRORS();
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	GL_CHECK_ERRORS();
	glEnableVertexAttribArray(0);
	GL_CHECK_ERRORS();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	GL_CHECK_ERRORS();
	glEnableVertexAttribArray(1);
	GL_CHECK_ERRORS();
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	GL_CHECK_ERRORS();
}

void Quad::render()
{
	glBindVertexArray(quadVAO);
	GL_CHECK_ERRORS();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	GL_CHECK_ERRORS_MSG("Quad glDrawElements");
	glBindVertexArray(0);
	GL_CHECK_ERRORS();
}
