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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));

	//Normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));

	//Binormal
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(9 * sizeof(GLfloat)));

	//Tangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(12 * sizeof(GLfloat)));

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

	int sides = 6;

	GLfloat dw = width / res;
	GLfloat dh = height / res;
	GLfloat dd = depth / res;

	vector<glm::vec3> side;
	side.push_back(glm::vec3(0)); //Front
	side.push_back(glm::vec3(width,0,-depth)); //Back
	side.push_back(glm::vec3(width,0,0)); //Right
	side.push_back(glm::vec3(0,0,-depth)); //Left
	side.push_back(glm::vec3(0,height,0)); //Top
	side.push_back(glm::vec3(0,0,-depth)); //Bottom

	vector<glm::vec3> signs;
	signs.push_back(glm::vec3(1,1,0));
	signs.push_back(glm::vec3(-1,1,0));
	signs.push_back(glm::vec3(0,1,-1));
	signs.push_back(glm::vec3(0,1,1));
	signs.push_back(glm::vec3(1,0,-1));
	signs.push_back(glm::vec3(1,0,1));

	vector<glm::vec3> normals;
	normals.push_back(glm::vec3(0, 0, 1));
	normals.push_back(glm::vec3(0, 0, -1));
	normals.push_back(glm::vec3(1, 0, 0));
	normals.push_back(glm::vec3(-1, 0, 0));
	normals.push_back(glm::vec3(0, 1, 0));
	normals.push_back(glm::vec3(0, -1, 0));

	vector<glm::vec3> binormals;
	binormals.push_back(glm::vec3(1, 0, 0));
	binormals.push_back(glm::vec3(-1, 0, 0));
	binormals.push_back(glm::vec3(0, 0, -1));
	binormals.push_back(glm::vec3(0, 0, 1));
	binormals.push_back(glm::vec3(1, 0, 0));
	binormals.push_back(glm::vec3(-1, 0, 0));


	for (int s = 0; s < sides; ++s) {
		glm::vec3 normal = normals[s];
		glm::vec3 binormal = binormals[s];
		glm::vec3 tangent = glm::cross(normal, binormal);
		auto pos = side[s];
		auto sign = signs[s];
		for (int i = 0; i < res + 1; ++i) {
			for (int j = 0; j < res + 1; ++j) {
				Vertex v;
				v.x = pos.x;
				v.y = pos.y;
				v.z = pos.z;

				v.nx = normal.x;
				v.ny = normal.y;
				v.nz = normal.z;

				v.bx = binormal.x;
				v.by = binormal.y;
				v.bz = binormal.z;

				v.tx = tangent.x;
				v.ty = tangent.y;
				v.tz = tangent.z;

				v.texx = (GLfloat) i / (res + 1);
				v.texy = (GLfloat) j / (res + 1);
				v.texz = 0;

				vertices->push_back(v);
				pos.x += sign.x * dw;
				if (sign.y != 0) {
					pos.z += sign.z * dd;
				}
				
			}
			pos.x = side[s].x;
			pos.y += sign.y * dh;
			if (sign.y != 0) {
				pos.z = side[s].z;
			}
			else {
				pos.z += sign.z * dd;
			}
		}
	}
	for (int s = 0; s < sides; s++) {
		for (int i = 0; i < res; ++i) {
			for (int j = 0; j < res; ++j) {
				Triangle t1, t2;
				auto curr = s*(res+1)*(res+1) +i*(res+1) + j;
				t1.a = curr;
				t1.b = curr + res + 2;
				t1.c = curr + res + 1;

				t2.a = curr;
				t2.b = curr + 1;
				t2.c = curr + res + 2;

				indices->push_back(t1);
				indices->push_back(t2);
			}
		}
	}
	
	return VertexArray::CreateVertexArray(vertices, indices);
}

VertexArray ParametricShapes::createSurface(GLfloat width, GLfloat height, GLint res)
{
	shared_ptr<vector<Vertex>> vertices = make_shared<vector<Vertex>>();
	shared_ptr<vector<Triangle>> indices = make_shared<vector<Triangle>>();

	//create surface
	return VertexArray::CreateVertexArray(vertices, indices);
}
