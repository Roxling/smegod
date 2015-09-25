#pragma once
#include "smegod_dep.h"
#include "nodes.h"
#include "textures.h"
#include "materials.h"

class Geometry abstract : public WorldObject {
public:
	Geometry(shared_ptr<ShaderGroup> mshader_group);
	glm::vec3 color;
	Material material = Material::DEFAULT;
	shared_ptr<Texture> texture = nullptr;
	void renderSelf(glm::mat4 combined_transform) override;
	virtual void render() {};
protected:
	GLuint VBO, VAO, EBO;
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
	GLint color_location;
};

class Cube : public Geometry {

public:
	Cube(shared_ptr<ShaderGroup> mshader_group);
	void render() override;
};

class Frame : public WorldObject {
public:
	Frame(shared_ptr<ShaderGroup> mshader_group);
};

