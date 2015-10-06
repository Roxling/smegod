#pragma once
#include "smegod_dep.h"
struct Vertex {
	GLfloat x, y, z,			/* Coordinate */
		texx, texy, texz,	/* texture coordinate */
		nx, ny, nz,			/* normal vector */
		tx, ty, tz,			/* tangent vector */
		bx, by, bz = 0;			/* binormal vector */
};
struct Triangle
{
	GLint a, b, c;		/* vertex indices */
};

class VertexArray {
public:
	GLuint VBO = 0, VAO = 0, EBO = 0;
	GLsizei num_indices = 0;
	static VertexArray CreateVertexArray(shared_ptr<vector<Vertex>> vertices, shared_ptr<vector<Triangle>> indices);
};


class ParametricShapes abstract {
public:
	static VertexArray createSphere(GLfloat radius, GLint res_theta, GLint res_phi, bool inv = false);
	static VertexArray createTorus(GLfloat radiusA, GLfloat radiusB, GLint res_theta, GLint res_phi);
	static VertexArray createCube(GLfloat side, GLint res);
	static VertexArray createBlock(GLfloat width, GLfloat height, GLfloat depth, GLint res);
	static VertexArray createSurface(GLfloat width, GLfloat height, GLint res);
	static VertexArray createInfSurface(GLfloat radius, GLint res_radius, GLint res_phi);
	static VertexArray createInfSurface2(GLfloat side, GLint res);
};