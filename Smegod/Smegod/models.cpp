#include "models.h"


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

	va.num_indices = (GLsizei)(sizeof(Triangle) / sizeof(GLint) * indices->size());
	glBindVertexArray(0);
	return va;
}


Model::Model(string file)
{
	if (!Globals::File_Exists(FOLDER+file)) {
		cout << "File " << FOLDER + file << " does not exist. Can not load model." << endl;
		return;
	}

	path = FOLDER + file.substr(0, file.find_last_of('/')) + '/';
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(FOLDER + file, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
		return;
	}

	processNode(scene->mRootNode, scene);
	
}

Model::Model(VertexArray va)
{
	Mesh mesh;
	mesh.va = va;
	meshes.push_back(mesh);
}

void Model::processNode(aiNode * node, const aiScene * scene)
{

	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene);
	}
	
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

void Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
	Mesh new_mesh;
	shared_ptr<vector<Vertex>> vertices = make_shared<vector<Vertex>>();
	shared_ptr<vector<Triangle>> indices = make_shared<vector<Triangle>>();

	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.x = mesh->mVertices[i].x; //Forced to be triangles & have normals
		vertex.y = mesh->mVertices[i].y;
		vertex.z = mesh->mVertices[i].z;

		vertex.nx = mesh->mNormals[i].x;
		vertex.ny = mesh->mNormals[i].y;
		vertex.nz = mesh->mNormals[i].z;
		
		if (mesh->mTangents != nullptr) {
			vertex.tx = mesh->mTangents[i].x;
			vertex.ty = mesh->mTangents[i].y;
			vertex.tz = mesh->mTangents[i].z;
		}
		else {
			vertex.tx = 0;
			vertex.ty = 0;
			vertex.tz = 0;
		}
		if (mesh->mBitangents != nullptr) {
			vertex.bx = mesh->mBitangents[i].x;
			vertex.by = mesh->mBitangents[i].y;
			vertex.bz = mesh->mBitangents[i].z;
		}
		else {
			vertex.bx = 0;
			vertex.by = 0;
			vertex.bz = 0;
		}

		if (mesh->mTextureCoords != nullptr && mesh->mTextureCoords[0] != nullptr) {
			vertex.texx = mesh->mTextureCoords[0][i].x;
			vertex.texy = mesh->mTextureCoords[0][i].y;
			vertex.texz = mesh->mTextureCoords[0][i].z;
		}
		else {
			vertex.texx = 0;
			vertex.texy = 0;
			vertex.texz = 0;
		}


		vertices->push_back(vertex);
	}
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		if (face.mNumIndices != 3) {
			cout << "A face in this model is not a triangle. Model will not load correctly." << endl;
		}
		else {

			Triangle triangle;
			triangle.a = face.mIndices[0];
			triangle.b = face.mIndices[1];
			triangle.c = face.mIndices[2];
			indices->push_back(triangle);
		}
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		
		aiString str;
		GLuint id;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &str) == AI_SUCCESS) {
			Texture text(path + str.C_Str(), false);
			id = text.texture_id;
		}
		else {
			id = Texture::getDefaults()->texture;
		}
		new_mesh.material->textures.push_back({"diffuse_texture", id});

		if (material->GetTexture(aiTextureType_HEIGHT, 0, &str) == AI_SUCCESS) {
			Texture bump(path + str.C_Str(), false);
			id = bump.texture_id;
		}
		else {
			id = Texture::getDefaults()->bump;
		}
		new_mesh.material->textures.push_back({"normal_texture", id});
		
		if (material->GetTexture(aiTextureType_SPECULAR, 0, &str) == AI_SUCCESS) {
			Texture spec(path + str.C_Str(), false);
			id = spec.texture_id;
		}
		else {
			id = 0; //TODO load default specmap.
		}

		new_mesh.material->textures.push_back({"specular_texture", id});
	}
	
	new_mesh.va = VertexArray::CreateVertexArray(vertices, indices);

	meshes.push_back(new_mesh);
}
