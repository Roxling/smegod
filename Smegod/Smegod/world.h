#pragma once
#include "gl_dep.h"
#include <memory>
#include <vector>
#include "camera.h"

using namespace std;

class Node abstract {
public:
	
	glm::mat4 world;
	vector<shared_ptr<Node>> children;

public:
	void translate(float dx, float dy, float dz);
	void render(glm::mat4 combined_transforms);
	virtual void renderSelf(glm::mat4 combined_transform) {};

	void attach(shared_ptr<Node> child);
};

class Group : public Node {
	//TODO: what should we have here?
};

class World {
private:
	glm::mat4 world_pos;
	shared_ptr<Node> head;
public:
	shared_ptr<Camera> active_camera;
	GLuint active_shader_program;
	World();
	void setActiveCamera(shared_ptr<Camera> cam);
	virtual void update(double delta);
	virtual void initiate();
	void render();
};
