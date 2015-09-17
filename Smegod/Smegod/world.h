#pragma once
#include "gl_dep.h"
#include <memory>
#include "camera.h"

using namespace std;

class Node abstract {
protected:
	glm::mat4 world;

public:
	void setTranslation(float x, float y, float z);
	virtual void render() {}
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
