#pragma once
#include "gl_dep.h"
#include <vector>
#include <memory>
#include "world.h"

class Geometry : Group {
protected:
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
};

class Cube : Geometry {
public:
	Cube();
};

