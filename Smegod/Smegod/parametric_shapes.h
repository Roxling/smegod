#pragma once
#include "smegod_dep.h"
#include "models.h"

class ParametricShapes abstract {
public:
	static VertexArray createSphere(GLfloat radius, GLint res_theta, GLint res_phi, bool inv = false);
	static VertexArray createTorus(GLfloat radiusA, GLfloat radiusB, GLint res_theta, GLint res_phi);
	static VertexArray createCube(GLfloat side, GLint res);
	static VertexArray createBlock(GLfloat width, GLfloat height, GLfloat depth, GLint res);
	static VertexArray createSurface(GLfloat width, GLfloat height, GLint res);
	static VertexArray createInfSurface(GLfloat radius, GLint res_radius, GLint res_phi);
	static VertexArray createInfSurface2(GLfloat side, GLint res);
	static VertexArray createCone(GLfloat radius, GLfloat height, GLint res);
};