#pragma once
#include "smegod_dep.h"
#include "nodes.h"
#include "textures.h"

class Geometry abstract : public WorldObject {
public:
	Geometry(shared_ptr<ShaderGroup> mshader_group);
	glm::vec3 color;
	shared_ptr<Texture> texture = nullptr;
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

