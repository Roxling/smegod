#pragma once
#include "GL\glew.h"

/* Cube vertices+indices defined in CCL order */
GLfloat c_vertices[] = {-.5f, .5f, .5f,
						 .5f, .5f, .5f,
						 .5f,-.5f, .5f,
						-.5f,-.5f, .5f,
						-.5f, .5f,-.5f,
						 .5f, .5f,-.5f,
						 .5f,-.5f,-.5f,
						-.5f,-.5f,-.5f, };

GLuint c_indices[] = {	0,2,1, 0,3,2,
						1,6,5, 1,2,6,
						5,7,4, 5,6,7,
						4,3,0, 4,7,3,
						4,1,5, 4,0,1,
						3,6,2, 3,7,6};