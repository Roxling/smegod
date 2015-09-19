#include "geometries.h"



Geometry::Geometry(GLuint mshader_program) : shader_program(mshader_program)
{
	world_location = glGetUniformLocation(shader_program, "world");
	color_location = glGetUniformLocation(shader_program, "incolor");
}

void Geometry::renderSelf(glm::mat4 combined_transform)
{
	glUniformMatrix4fv(world_location, 1, GL_FALSE, glm::value_ptr(combined_transform));
	render();
}

Cube::Cube(GLuint mshader_program) : Geometry(mshader_program)
{
	vertices = vector<GLfloat>{
		//Position			//Texture coord
		-.5f, .5f, .5f,		0.0f, 1.0f,
		.5f, .5f, .5f,		1.0f, 1.0f,
		.5f, -.5f, .5f,		1.0f, 0.0f,
		-.5f, -.5f, .5f,	0.0f, 0.0f,
		-.5f, .5f, -.5f,	1.0f, 1.0f,
		.5f, .5f, -.5f,		0.0f, 1.0f,
		.5f, -.5f, -.5f,	0.0f, 0.0f,
		-.5f, -.5f, -.5f, 	1.0f, 0.0f };
	indices = vector<GLuint>{
		0, 2, 1,  0, 3, 2,
		1, 6, 5,  1, 2, 6,
		5, 7, 4,  5, 6, 7,
		4, 3, 0,  4, 7, 3,
		4, 1, 5,  4, 0, 1,
		3, 6, 2,  3, 7, 6 };

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);

	//Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Texture pos
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Cube::render()
{
	glUseProgram(shader_program);
	glUniform3fv(color_location, 1, glm::value_ptr(color));
	if (texture != nullptr) {
		glBindTexture(GL_TEXTURE_2D, texture->texture_id);
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)(sizeof(indices[0]) * indices.size() + sizeof(vertices[0]) * vertices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
