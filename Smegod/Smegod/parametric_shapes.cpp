#include "parametric_shapes.h"

VertexArray ParametricShapes::createSphere(GLfloat radius, GLint res_theta, GLint res_phi, bool inv)
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
			t2.a = curr + 1;

			if (inv) {
				t1.c = curr + 1;
				t1.b = curr + res_theta;
				t2.c = curr + res_theta + 1;
				t2.b = curr + res_theta;
			}
			else {
				t1.b = curr + 1;
				t1.c = curr + res_theta;
				t2.b = curr + res_theta + 1;
				t2.c = curr + res_theta;
			}


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

VertexArray ParametricShapes::createSurface(GLfloat width, GLfloat height, GLint xres, GLint yres) {
	shared_ptr<vector<Vertex>> vertices = make_shared<vector<Vertex>>();
	shared_ptr<vector<Triangle>> indices = make_shared<vector<Triangle>>();

	glm::vec3 normal = { 0,1,0 };
	glm::vec3 binormal = { 1,0,0 };
	glm::vec3 tangent = { 0,0,1 };

	GLfloat dw = width / xres;
	GLfloat dh = height / yres;

	GLfloat W = 0;
	GLfloat H = 0;
	for (int i = 0; i < yres + 1; ++i) {
		for (int j = 0; j < xres + 1; ++j) {
			Vertex v;
			v.x = W;
			v.y = 0;
			v.z = H;

			v.nx = normal.x;
			v.ny = normal.y;
			v.nz = normal.z;

			v.bx = binormal.x;
			v.by = binormal.y;
			v.bz = binormal.z;

			v.tx = tangent.x;
			v.ty = tangent.y;
			v.tz = tangent.z;

			v.texx = (GLfloat)i*0.7f;
			v.texy = (GLfloat)j*0.7f;
			v.texz = 0;

			vertices->push_back(v);
			W += dw;

		}
		W = 0;
		H += dh;
	}


	for (int i = 0; i < yres; ++i) {
		for (int j = 0; j < xres; ++j) {
			Triangle t1, t2;
			auto curr = i*(xres + 1) + j;
			t1.a = curr;
			t1.c = curr + xres + 2;
			t1.b = curr + xres + 1;

			t2.a = curr;
			t2.c = curr + 1;
			t2.b = curr + xres + 2;

			indices->push_back(t1);
			indices->push_back(t2);
		}
	}

	return VertexArray::CreateVertexArray(vertices, indices);
}

VertexArray ParametricShapes::createSurface(GLfloat width, GLfloat height)
{
	return ParametricShapes::createSurface(width, height, (int)floor(width), (int)floor(height));
}
VertexArray ParametricShapes::createSurface(GLfloat width, GLfloat height, GLint res)
{
	return ParametricShapes::createSurface(width, height, res, res);
}

VertexArray ParametricShapes::createInfSurface(GLfloat radius, GLint res_radius, GLint res_phi)
{
	shared_ptr<vector<Vertex>> vertices = make_shared<vector<Vertex>>();
	shared_ptr<vector<Triangle>> indices = make_shared<vector<Triangle>>();

	glm::vec3 normal = { 0,1,0 };
	glm::vec3 binormal = { 1,0,0 };
	glm::vec3 tangent = { 0,0,1 };

	GLfloat pi = glm::pi<float>();
	GLfloat dR = radius / res_radius;
	GLfloat R = 0;
	GLfloat dP = 2 * pi / res_phi;
	GLfloat P = 0;


	for (int i = 0; i < res_radius + 1; ++i) {
		for (int j = 0; j < res_phi + 1; ++j) {
			Vertex v;
			v.x = cos(P)*R;
			v.y = 0;
			v.z = sin(P)*R;

			v.nx = normal.x;
			v.ny = normal.y;
			v.nz = normal.z;

			v.bx = binormal.x;
			v.by = binormal.y;
			v.bz = binormal.z;

			v.tx = tangent.x;
			v.ty = tangent.y;
			v.tz = tangent.z;

			v.texx = (GLfloat)i / (res_phi + 1);
			v.texy = (GLfloat)j / (res_radius + 1);
			v.texz = 0;

			vertices->push_back(v);
			P += dP;
		}
		P= 0;
		R += dR *i / res_radius;
	}


	for (int i = 0; i < res_radius; ++i) {
		for (int j = 0; j < res_phi; ++j) {
			Triangle t1, t2;
			auto curr = i*(res_phi + 1) + j;
			t1.a = curr;
			t1.b = curr + res_phi + 2;
			t1.c = curr + res_phi + 1;

			t2.a = curr;
			t2.b = curr + 1;
			t2.c = curr + res_phi + 2;

			indices->push_back(t1);
			indices->push_back(t2);
		}
	}

	return VertexArray::CreateVertexArray(vertices, indices);
}
VertexArray ParametricShapes::createInfSurface2(GLfloat side, GLint res)
{
	shared_ptr<vector<Vertex>> vertices = make_shared<vector<Vertex>>();
	shared_ptr<vector<Triangle>> indices = make_shared<vector<Triangle>>();

	glm::vec3 normal = { 0,1,0 };
	glm::vec3 binormal = { 1,0,0 };
	glm::vec3 tangent = { 0,0,1 };

	GLfloat dw = side / res;
	GLfloat dh = side / res;

	GLfloat inf_step = 1;
	GLfloat inf_step_w = 1;

	GLfloat W = 0;
	GLfloat H = 0;
	for (int i = 0; i < res + 1; ++i) {
		inf_step_w = 1;
		for (int j = 0; j < res + 1; ++j) {
			Vertex v;
			v.x = W;
			v.y = 0;
			v.z = H;

			v.nx = normal.x;
			v.ny = normal.y;
			v.nz = normal.z;

			v.bx = binormal.x;
			v.by = binormal.y;
			v.bz = binormal.z;

			v.tx = tangent.x;
			v.ty = tangent.y;
			v.tz = tangent.z;

			v.texx = (GLfloat)i / (res + 1);
			v.texy = (GLfloat)j / (res + 1);
			v.texz = 0;

			inf_step_w +=0.5;

			vertices->push_back(v);
			W += dw * inf_step_w;

		}
		inf_step+= 0.5;

		W = 0;
		H += dh * inf_step;
	}


	for (int i = 0; i < res; ++i) {
		for (int j = 0; j < res; ++j) {
			Triangle t1, t2;
			auto curr = i*(res + 1) + j;
			t1.a = curr;
			t1.c = curr + res + 2;
			t1.b = curr + res + 1;

			t2.a = curr;
			t2.c = curr + 1;
			t2.b = curr + res + 2;

			indices->push_back(t1);
			indices->push_back(t2);
		}
	}

	return VertexArray::CreateVertexArray(vertices, indices);
}

VertexArray ParametricShapes::createCone(GLfloat radius, GLfloat height, GLint res)
{
	shared_ptr<vector<Vertex>> vertices = make_shared<vector<Vertex>>();
	shared_ptr<vector<Triangle>> indices = make_shared<vector<Triangle>>();


	GLfloat dP = 2 * glm::pi<float>() / res;
	GLfloat P = 0;
	float R = radius;
	float H = 0;
	for (int i = 0; i < 2; ++i) {

		for (int j = 0; j < res + 1; ++j) {
			Vertex v;
			v.x = cos(P)*R;
			v.y = H;
			v.z = sin(P)*R;

			v.nx = v.x * height/radius;
			v.ny = radius/height;
			v.nz = v.z * height / radius;

			glm::vec3 h_vec(0, height, 0);
			auto binormal = h_vec - glm::vec3(v.x, 0, v.z);

			v.bx = binormal.x;
			v.by = binormal.y;
			v.bz = binormal.z;

			auto tangent = glm::cross(glm::vec3(v.nx, v.ny, v.nz), glm::vec3(v.bx, v.by, v.bz ));

			v.tx = tangent.x;
			v.ty = tangent.y;
			v.tz = tangent.z;

			v.texx = (GLfloat)i / (res + 1);
			v.texy = (GLfloat)j / (res + 1);
			v.texz = 0;


			vertices->push_back(v);
			P += dP;

		}
		P = 0;
		H = height;
		R = 0;
	}


	for (int j = 0; j < res; ++j) {
		Triangle t1, t2;
		auto curr = j;
		t1.a = curr;
		t1.c = curr + res + 1;
		t1.b = curr + res;

		t2.a = curr;
		t2.c = curr + 1;
		t2.b = curr + res + 1;

		indices->push_back(t1);
		indices->push_back(t2);
	}

	return VertexArray::CreateVertexArray(vertices, indices);
}

VertexArray ParametricShapes::createNDCQuad(GLfloat x, GLfloat y, GLfloat width, GLfloat height)
{
	shared_ptr<vector<Vertex>> vertices = make_shared<vector<Vertex>>();
	shared_ptr<vector<Triangle>> indices = make_shared<vector<Triangle>>();
	Vertex bl, br, tl, tr;
	bl.x = x;
	bl.y = y;
	bl.z = 0;
	bl.texx = 0;
	bl.texy = 0;
	bl.texz = 0;

	br.x = x+width;
	br.y = y;
	br.z = 0;
	br.texx = 1;
	br.texy = 0;
	br.texz = 0;

	tl.x = x;
	tl.y = y+height;
	tl.z = 0;
	tl.texx = 0;
	tl.texy = 1;
	tl.texz = 0;

	tr.x = x + width;
	tr.y = y + height;
	tr.z = 0;
	tr.texx = 1;
	tr.texy = 1;
	tr.texz = 0;

	vertices->push_back(bl); //0
	vertices->push_back(br); //1
	vertices->push_back(tl); //2
	vertices->push_back(tr); //3
	Triangle t1, t2;
	t1.a = 2;
	t1.b = 0;
	t1.c = 3;
	
	t2.a = 0;
	t2.b = 1;
	t2.c = 3;

	indices->push_back(t1);
	indices->push_back(t2);

	return VertexArray::CreateVertexArray(vertices, indices);
}
