#pragma once
#include "smegod_dep.h"
#include "textures.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex {
	GLfloat x, y, z,			/* Coordinate */
		texx, texy, texz,	/* texture coordinate */
		nx, ny, nz,			/* normal vector */
		tx, ty, tz,			/* tangent vector */
		bx, by, bz = 0;			/* binormal vector */
};
struct Triangle
{
	GLint a, b, c = 0;		/* vertex indices */
};

class VertexArray {
public:
	GLuint VBO = 0, VAO = 0, EBO = 0;
	GLsizei num_indices = 0;
	static VertexArray CreateVertexArray(shared_ptr<vector<Vertex>> vertices, shared_ptr<vector<Triangle>> indices);
};

class Mesh {
public:
	Texture texture;
	Texture bumpmap;
	Texture specular;
	VertexArray va;
};

class Model {
public:
	Model(string file);
	Model(VertexArray va);
	vector<Mesh> meshes;
private:
	const string FOLDER = "Models/";
	string path = "";
	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh, const aiScene* scene);
};


