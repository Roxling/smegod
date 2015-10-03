#pragma once
#include "smegod_dep.h"
#include "nodes.h"
#include "textures.h"
#include "materials.h"
#include "parametric_shapes.h"

class Geometry : public WorldObject {
public:
	Geometry::Geometry(shared_ptr<ShaderGroup> mshader_group, VertexArray va);
	Material material = Material::DEFAULT;
	shared_ptr<Texture> texture = Texture::DEFAULT;
	shared_ptr<Texture> bumpmap = Texture::DEFAULT;
	glm::vec3 color = { 1.f,1.f,1.f };
	virtual void renderSelf();
	void render(glm::mat4 combined_transform) override;
protected:
	//Geometry::Geometry(shared_ptr<ShaderGroup> mshader_group) : WorldObject(mshader_group) {}
	unique_ptr<VertexArray> vertex_array;
};

class Frame : public WorldObject {
public:
	Frame(shared_ptr<ShaderGroup> mshader_group);
};

