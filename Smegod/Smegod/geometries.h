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
	Geometry(shared_ptr<ShaderGroup> mshader_group, VertexArray va);
	Geometry(shared_ptr<ShaderGroup> mshader_group, Model model);
	
	virtual void renderSelf(Mesh &mesh);
	void render(glm::mat4 combined_transform) override;
	void setColor(glm::vec3 rgb);
protected:
	unique_ptr<Model> model;
};

class Frame : public WorldObject {
public:
	Frame(shared_ptr<ShaderGroup> mshader_group);
};

class Skybox : public WorldObject {
public:
	Skybox(shared_ptr<Cubemap> cubemap);
	void render(glm::mat4 combined_transform) override;
private:
	shared_ptr<Cubemap> cubemap;
	shared_ptr<VertexArray> skybox;
};