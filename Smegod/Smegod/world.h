#pragma once
#include "gl_dep.h"
#include <memory>
#include <vector>
#include "camera.h"

using namespace std;

class Node abstract {
protected:
	glm::mat4 world;
	vector<shared_ptr<Node>> children;

public:
	void translate(float dx, float dy, float dz);
	void render(GLuint shader_program);
	virtual void renderSelf() {};

	void attach(shared_ptr<Node> child);
};

class Group : public Node {
public:
	Group() {};
};

class World {
private:
	shared_ptr<Node> head;
public:
	shared_ptr<Camera> active_camera;
	GLuint active_shader_program;
	World();
	void setActiveCamera(shared_ptr<Camera> cam);
	virtual void update();
	virtual void initiate();
	void render();
};
