#include "parametric_shapes.h"

VertexArray VertexArray::CreateVertexArray(shared_ptr<vector<Vertex>> vertices, shared_ptr<vector<Triangle>> indices)
{
	VertexArray va;

	glGenVertexArrays(1, &va.VAO);
	glGenBuffers(1, &va.VBO);
	glGenBuffers(1, &va.EBO);
	glBindVertexArray(va.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, va.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices->size(), vertices->data(), GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, va.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle) * indices->size(), indices->data(), GL_STATIC_DRAW);

	//Vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	//Texture pos
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(1 * sizeof(Vertex)));

	//Normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(2 * sizeof(Vertex)));

	//Binormal
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(Vertex)));

	//Tangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(4 * sizeof(Vertex)));

	va.num_indices = (GLsizei)( sizeof(Triangle) / sizeof(GLint) * indices->size());
	glBindVertexArray(0);
	return va;
}

VertexArray ParametricShapes::createSphere(GLfloat radius, GLint res_theta, GLint res_phi)
{
	shared_ptr<vector<Vertex>> vertices = make_shared<vector<Vertex>>();
	shared_ptr<vector<Triangle>> indices = make_shared<vector<Triangle>>();

	GLfloat dT = glm::two_pi<GLfloat>() / (res_theta - 1);
	GLfloat dP = glm::pi<GLfloat>() / (res_phi - 1);

	GLfloat T = 0.f;
	GLfloat P = 0.f;

	//Create vertices
	for (int j = 0; j < res_phi; ++j) {
		for (int i = 0; i < res_theta; ++i) {
			Vertex v;
			//Coords
			glm::vec3 t(radius * cosf(T) * sin(P), 0, -radius * sinf(T) * sin(P));		
			t = glm::normalize(t);

			glm::vec3 b(radius * sinf(T)* cosf(P), radius * sinf(P), radius * cosf(T)* cosf(P));
			b = glm::normalize(b);

			glm::vec3 n = glm::cross(glm::vec3(radius * cosf(T), 0, -radius * sinf(T)),b);
			
			n = glm::normalize(n);

			v.x = radius * sinf(T) * sinf(P);
			v.y = -radius * cosf(P);
			v.z = radius * cosf(T) * sinf(P);

			v.tx = t.x;
			v.ty = t.y;
			v.tz = t.z;

			v.bx = b.x;
			v.by = b.y;
			v.bz = b.z;


			v.nx = n.x;
			v.ny = n.y;
			v.nz = n.z;

			v.texx = T / glm::two_pi<GLfloat>();
			v.texy = P / glm::pi<GLfloat>();
			v.texz = 0;

			vertices->push_back(v);
			T += dT;
		}
		P += dP;
		T = 0.f;
	}

	//Create triangles
	for (int j = 0; j < res_phi - 1; ++j) {
		for (int i = 0; i < res_theta - 1; ++i) {
			Triangle t1, t2;
			auto curr = j*res_phi + i;
			t1.a = curr;
			t1.b = curr + 1;
			t1.c = curr + res_theta;

			t2.a = curr + 1;
			t2.b = curr + res_theta + 1;
			t2.c = curr + res_theta;


			indices->push_back(t1);
			indices->push_back(t2);
		}
	}

	return VertexArray::CreateVertexArray(vertices,indices);
}

VertexArray ParametricShapes::createTorus(GLfloat radiusA, GLfloat radiusB, GLint res_theta, GLint res_phi)
{
	shared_ptr<vector<Vertex>> vertices = make_shared<vector<Vertex>>();
	shared_ptr<vector<Triangle>> indices = make_shared<vector<Triangle>>();

	GLfloat dT = glm::two_pi<GLfloat>() / (res_theta - 1);
	GLfloat dP = glm::two_pi<GLfloat>() / (res_phi - 1);

	GLfloat T = 0.f;
	GLfloat P = 0.f;

	//Create vertices
	for (int j = 0; j < res_phi; ++j) {
		for (int i = 0; i < res_theta; ++i) {
			Vertex v;
			glm::vec3 t(-radiusB * sinf(T)* cosf(P), -radiusB * sinf(T)* sinf(P), -radiusB * cosf(T));
			t = glm::normalize(t);

			glm::vec3 b(-(radiusA + radiusB * cosf(T))* sinf(P), (radiusA + radiusB * cosf(T))* cosf(P), 0);
			b = glm::normalize(b);

			glm::vec3 n = glm::cross(t, b);
			//Coords
			v.x = (radiusA + radiusB*cosf(T))*cosf(P);
			v.y = (radiusA + radiusB*cosf(T))*sinf(P);
			v.z = -radiusB * sinf(T);

			v.tx = t.x;
			v.ty = t.y;
			v.tz = t.z;

			v.bx = b.x;
			v.by = b.y;
			v.bz = b.z;


			v.nx = n.x;
			v.ny = n.y;
			v.nz = n.z;

			v.texx = T / glm::two_pi<GLfloat>();
			v.texy = P / glm::two_pi<GLfloat>();
			v.texz = 0;


			vertices->push_back(v);
			T += dT;
		}
		P += dP;
		T = 0.f;
	}

	//Create triangles
	for (int j = 0; j < res_phi - 1; ++j) {
		for (int i = 0; i < res_theta - 1; ++i) {
			Triangle t1, t2;
			auto curr = j*res_phi + i;
			t1.a = curr;
			t1.b = curr + 1;
			t1.c = curr + res_theta;

			t2.a = curr + 1;
			t2.b = curr + res_theta + 1;
			t2.c = curr + res_theta;

			indices->push_back(t1);
			indices->push_back(t2);
		}
	}

	return VertexArray::CreateVertexArray(vertices, indices);
}

VertexArray ParametricShapes::createCube(GLfloat side, GLint res)
{
	return createBlock(side,side,side,res);
}

VertexArray ParametricShapes::createBlock(GLfloat width, GLfloat height, GLfloat depth, GLint res)
{
	shared_ptr<vector<Vertex>> vertices = make_shared<vector<Vertex>>();
	shared_ptr<vector<Triangle>> indices = make_shared<vector<Triangle>>();

	GLfloat dw = width / res;
	GLfloat dh = height / res;
	GLfloat dd = depth / res;

	//TODO create block

	return VertexArray::CreateVertexArray(vertices, indices);
}
