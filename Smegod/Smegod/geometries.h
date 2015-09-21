#pragma once
#include "smegod_dep.h"
#include "nodes.h"
#include "textures.h"

class Geometry abstract : public WorldObject {
public:
	Geometry(GLuint mshader_program);
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
	Cube(GLuint mshader_program);
	void render() override;
};

