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
private:
	GLint world_location;
protected:
	shared_ptr<ShaderGroup> shader_group;
public:
	WorldObject(shared_ptr<ShaderGroup> mshader_group);
	void renderSelf(glm::mat4 combined_transform) override;
	virtual void render() {};
};