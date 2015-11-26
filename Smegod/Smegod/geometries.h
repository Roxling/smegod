#pragma once
#include "smegod_dep.h"
#include "nodes.h"
#include "textures.h"
#include "cubemap.h"
#include "materials.h"
#include "parametric_shapes.h"
#include "models.h"

class Geometry : public WorldObject {
public:
	Geometry(VertexArray va);
	Geometry(Model model);
	
	virtual void renderSelf(Mesh &mesh);
	void render(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader) override;
	void setColor(glm::vec3 rgb);
	void bindTexture(string glslName, GLuint id);
	unique_ptr<Model> model;
protected:
};

class Frame : public WorldObject {
public:
	Frame(shared_ptr<ShaderGroup> mshader_group);
};

class Skybox : public WorldObject {
public:
	Skybox(shared_ptr<Cubemap> cubemap);
	void render(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader) override;
private:
	shared_ptr<Cubemap> cubemap;
	shared_ptr<VertexArray> skybox;
};


class Quad {
public:
	Quad();
	Quad(glm::vec2 &lb, glm::vec2 &tr);
	Quad(float x1, float y1, float x2, float y2);

	void render();

private:
	GLuint quadVAO = 0;
	GLuint quadVBO;
};