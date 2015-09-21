#pragma once
#include "smegod_dep.h"
#include "geometries.h"

class Light : public Geometry {
public:
	Light(GLuint mshader_program);
	void render() override;
};
