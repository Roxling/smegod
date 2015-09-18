#pragma once
#include "gl_dep.h"
#include <vector>
#include <memory>
#include "world.h"

class Geometry abstract : public Group {
public:
	Geometry(GLuint mshader_program) : shader_program(mshader_program) {}
	glm::vec3 color;
	void renderSelf(glm::mat4 combined_transform) override;
	virtual void render() = 0;
protected:
	GLuint shader_program;
	GLuint VBO, VAO, EBO;
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
};

class Cube : public Geometry {

public:
	Cube(GLuint mshader_program);
	void render() override;
};

