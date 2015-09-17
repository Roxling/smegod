#pragma once
#include <memory>
#include "camera.h"
#include "GLM\glm.hpp"

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
	World();
	void setActiveCamera(shared_ptr<Camera> &cam);
	virtual void update();
	void render();
};
