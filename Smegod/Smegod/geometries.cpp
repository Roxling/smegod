#include "geometries.h"

Geometry::Geometry(shared_ptr<ShaderGroup> mshader_group) : WorldObject(mshader_group)
{
	color_location = glGetUniformLocation(mshader_group->getProgram(), "incolor");
}

void Geometry::renderSelf(glm::mat4 combined_transform)
{
	shader_group->use();
	auto program = shader_group->getProgram();
	auto wIT = glm::transpose(glm::inverse(combined_transform)); //is this the correct way to calculate the inverse transpose?
	glUniformMatrix4fv(world_location, 1, GL_FALSE, glm::value_ptr(combined_transform));
	glUniformMatrix4fv(worldIT_location, 1, GL_FALSE, glm::value_ptr(wIT));

	//material
	GLuint ambient_loc, diffuse_loc, specular_loc, shininess_loc; //for fs
	ambient_loc = glGetUniformLocation(program, "kambient");
	glUniform3fv(ambient_loc, 1, glm::value_ptr(material.ambient));
	diffuse_loc = glGetUniformLocation(program, "kdiffuse");
	glUniform3fv(diffuse_loc, 1, glm::value_ptr(material.diffuse));
	specular_loc = glGetUniformLocation(program, "kspecular");
	glUniform3fv(specular_loc, 1, glm::value_ptr(material.specular));
	shininess_loc = glGetUniformLocation(program, "shininess");
	glUniform1fv(shininess_loc, 1, (const float *)&material.shininess);
	render();
}

Cube::Cube(shared_ptr<ShaderGroup> mshader_group) : Geometry(mshader_group)
{
	vertices = vector<GLfloat>{
		//Position			//Texture coord		//Normal
		//Front (0-3)
		-.5f, .5f, .5f,		0.0f, 1.0f,			0.0f, 0.0f, 1.0f, //0
		.5f, .5f, .5f,		1.0f, 1.0f,			0.0f, 0.0f, 1.0f, //1
		.5f, -.5f, .5f,		1.0f, 0.0f,			0.0f, 0.0f, 1.0f, //2
		-.5f, -.5f, .5f,	0.0f, 0.0f,			0.0f, 0.0f, 1.0f, //3
		//Back (4-7)
		.5f, .5f, -.5f,		0.0f, 1.0f,			0.0f, 0.0f, -1.0f, //4
		-.5f, .5f, -.5f,	1.0f, 1.0f,			0.0f, 0.0f, -1.0f, //5
		-.5f, -.5f, -.5f, 	1.0f, 0.0f,			0.0f, 0.0f, -1.0f, //6
		.5f, -.5f, -.5f,	0.0f, 0.0f,			0.0f, 0.0f, -1.0f, //7
		//Right (8-11) 
		.5f, .5f, .5f,		0.0f, 1.0f,			1.0f, 0.0f, 0.0f, //8
		.5f, .5f, -.5f,		1.0f, 0.0f,			1.0f, 0.0f, 0.0f,//9
		.5f, -.5f, -.5f,	0.0f, 0.0f,			1.0f, 0.0f, 0.0f,//10
		.5f, -.5f, .5f,		1.0f, 1.0f,			1.0f, 0.0f, 0.0f, //11
		//Left (12-15)
		-.5f, .5f, -.5f,	0.0f, 1.0f,			-1.0f, 0.0f, 0.0f,//12
		-.5f, .5f, .5f,		1.0f, 1.0f,			-1.0f, 0.0f, 0.0f, //13
		-.5f, -.5f, .5f,	1.0f, 0.0f,			-1.0f, 0.0f, 0.0f, //14
		-.5f, -.5f, -.5f, 	0.0f, 0.0f,			-1.0f, 0.0f, 0.0f, //15
		//Top (16-19)
		-.5f, .5f, -.5f,	0.0f, 1.0f,			0.0f, 1.0f, 0.0f, //16
		.5f, .5f, -.5f,		1.0f, 1.0f,			0.0f, 1.0f, 0.0f, //17
		.5f, .5f, .5f,		1.0f, 0.0f,			0.0f, 1.0f, 0.0f, //18
		-.5f, .5f, .5f,		0.0f, 0.0f,			0.0f, 1.0f, 0.0f, //19
		//Bottom (20-23)
		-.5f, -.5f, .5f,	0.0f, 1.0f,			0.0f, -1.0f, 0.0f, //20
		.5f, -.5f, .5f,		1.0f, 1.0f,			0.0f, -1.0f, 0.0f, //21
		.5f, -.5f, -.5f,	1.0f, 0.0f,			0.0f, -1.0f, 0.0f, //22
		-.5f, -.5f, -.5f,	0.0f, 0.0f,			0.0f, -1.0f, 0.0f };//23
	indices = vector<GLuint>{
		0, 2, 1,	 0, 3, 2, //Front
		4, 6, 5,	 4, 7, 6, //Back
		8, 10, 9,    8, 11, 10, //Right
		12, 14, 13,  12, 15, 14,  //Left
		16, 18, 17,  16, 19, 18, //Top
		20, 22, 21,  20, 23, 22 //Bottom
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	//Vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);

	//Texture pos
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	//Normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));

	glBindVertexArray(0);
}

void Cube::render()
{
	glUniform3fv(color_location, 1, glm::value_ptr(color));
	if (texture != nullptr) {
		glBindTexture(GL_TEXTURE_2D, texture->texture_id);
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES,(GLsizei) indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

Frame::Frame(shared_ptr<ShaderGroup> mshader_group) : WorldObject(mshader_group)
{
	//X
	auto axis = make_shared<Cube>(mshader_group);
	axis->world = glm::scale(axis->world, glm::vec3(2.f,0.1f,0.1f));
	axis->translate(.5f, 0, 0);
	axis->color = { 1.f, 0, 0 };
	attach(axis);
	//Y
	axis = make_shared<Cube>(mshader_group);
	axis->world = glm::scale(axis->world, glm::vec3(0.1f, 2.f, 0.1f));
	axis->translate(0, .5f, 0);
	axis->color = { 0, 1.f, 0 };
	attach(axis);
	//Z
	axis = make_shared<Cube>(mshader_group);
	axis->world = glm::scale(axis->world, glm::vec3(0.1f, 0.1f, 2.f));
	axis->translate(0, 0, .5f);
	axis->color = { 0, 0, 1.f};
	attach(axis);

	//box
	axis = make_shared<Cube>(mshader_group);
	axis->world = glm::scale(axis->world, glm::vec3(.2f));
	axis->color = glm::vec3{ 0.f };
	attach(axis);
}
