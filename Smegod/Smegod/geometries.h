#pragma once
#include <vector>
#include <memory>
#include "world.h"
#include "GL\glew.h"

class Geometry : Group {
protected:
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
};

class Cube : Geometry {
public:
	Cube();
};

