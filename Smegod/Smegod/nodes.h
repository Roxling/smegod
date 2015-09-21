#pragma once
#include "smegod_dep.h"

class Node {
public:

	glm::mat4 world;
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
	GLuint shader_program;
public:
	WorldObject(GLuint mshader_program);
	void renderSelf(glm::mat4 combined_transform) override;
	virtual void render() = 0;
	//TODO: what should we have here?
};