#pragma once
#include "smegod_dep.h"
#include "nodes.h"
#include "textures.h"
#include "materials.h"

struct Vertex {
	GLfloat
		x, y, z,	// Coordinate
		nx, ny, nz, // Normal
		tx, ty, tz  // Texture Coordinate
		;
};

class Geometry abstract : public WorldObject {
public:
	Geometry::Geometry(shared_ptr<ShaderGroup> mshader_group) : WorldObject(mshader_group) {}
	glm::vec3 color;
	Material material = Material::DEFAULT;
	shared_ptr<Texture> texture = nullptr;
	virtual void renderSelf() {};
	void render(glm::mat4 combined_transform) override;
protected:
	GLuint VBO = 0, VAO = 0, EBO = 0;
	vector<GLfloat> vertices;
	vector<GLuint> indices;
};

class Cube : public Geometry {

public:
	Cube(shared_ptr<ShaderGroup> mshader_group);
	void renderSelf() override;
};

class Frame : public WorldObject {
public:
	Frame(shared_ptr<ShaderGroup> mshader_group);
};

