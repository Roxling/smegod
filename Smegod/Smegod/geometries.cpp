#include "geometries.h"

static GLfloat c_vertices[] = { -.5f, .5f, .5f,
.5f, .5f, .5f,
.5f,-.5f, .5f,
-.5f,-.5f, .5f,
-.5f, .5f,-.5f,
.5f, .5f,-.5f,
.5f,-.5f,-.5f,
-.5f,-.5f,-.5f, };

static GLuint c_indices[] = { 0,2,1, 0,3,2,
1,6,5, 1,2,6,
5,7,4, 5,6,7,
4,3,0, 4,7,3,
4,1,5, 4,0,1,
3,6,2, 3,7,6 };


Cube::Cube()
{
	vertices = vector<GLfloat>{
		-.5f, .5f, .5f,
		.5f, .5f, .5f,
		.5f, -.5f, .5f,
		-.5f, -.5f, .5f,
		-.5f, .5f, -.5f,
		.5f, .5f, -.5f,
		.5f, -.5f, -.5f,
		-.5f, -.5f, -.5f };
	indices = vector<GLuint>{
		0, 2, 1,  0, 3, 2,
		1, 6, 5,  1, 2, 6,
		5, 7, 4,  5, 6, 7,
		4, 3, 0,  4, 7, 3,
		4, 1, 5,  4, 0, 1,
		3, 6, 2,  3, 7, 6};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(c_vertices), c_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(c_indices), c_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Cube::renderSelf()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)(sizeof(c_indices) + size(c_vertices)), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
