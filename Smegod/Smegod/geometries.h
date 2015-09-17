#pragma once
#include "gl_dep.h"
#include <vector>
#include <memory>
#include "world.h"

class Geometry : public Group {
public:
	glm::vec3 color;
protected:
	GLuint VBO, VAO, EBO;

	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
};

class Cube : public Geometry {

public:
	Cube();
	void renderSelf(GLuint shader_program) override;
};

