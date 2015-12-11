#pragma once
#include "smegod_dep.h"
#include "shaders.h"

class Node {
public:

	glm::mat4 world = glm::mat4();
	glm::mat4 combined_world = glm::mat4();
	vector<shared_ptr<Node>> children;

public:
	void translate(float dx, float dy, float dz);
	void setTranslate(float x, float y, float z);

	void rotate(float dyaw, float dpitch, float droll);
	void scale(float x, float y, float z);
	void scale(float factor);

	glm::vec3 getPosition();

	void render(glm::mat4 combined_transforms, shared_ptr<ShaderGroup> shader, bool renderMaterials);
	virtual void renderSelf(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader, bool renderMaterials) {};

	virtual void attach(shared_ptr<Node> child);
};

class WorldObject : public Node {
protected:
	GLint world_location = 0;
	GLint worldIT_location = 0;
public:
	WorldObject(){}
	void renderSelf(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader, bool renderMaterials) override;
	virtual void render(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader, bool renderMaterials) {};
};

class ShaderCollection abstract {
protected:
	vector<shared_ptr<ShaderGroup>> shader_groups;
public:
	void addShaderGroup(shared_ptr<ShaderGroup> sg) { shader_groups.push_back(sg); }
};