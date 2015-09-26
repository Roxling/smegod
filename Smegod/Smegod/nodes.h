#pragma once
#include "smegod_dep.h"
#include "shaders.h"

class Node {
public:

	glm::mat4 world = glm::mat4();
	vector<shared_ptr<Node>> children;

public:
	void translate(float dx, float dy, float dz);
	void render(glm::mat4 combined_transforms);
	virtual void renderSelf(glm::mat4 combined_transform) {};

	void attach(shared_ptr<Node> child);
};

class WorldObject : public Node {
protected:
	GLint world_location = 0;
	GLint worldIT_location = 0;
	shared_ptr<ShaderGroup> shader_group;
public:
	WorldObject(shared_ptr<ShaderGroup> mshader_group) : shader_group(mshader_group) {}
	void renderSelf(glm::mat4 combined_transform) override;
	virtual void render(glm::mat4 combined_transform) {};
};

class ShaderCollection abstract {
protected:
	vector<shared_ptr<ShaderGroup>> shader_groups;
public:
	void addShaderGroup(shared_ptr<ShaderGroup> sg) { shader_groups.push_back(sg); }
};