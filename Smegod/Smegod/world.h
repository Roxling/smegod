#pragma once
#include "smegod_dep.h"
#include "camera.h"
#include "nodes.h"

class World abstract{
protected:
	shared_ptr<Node> head;
	glm::mat4 world_pos;
public:
	shared_ptr<Camera> active_camera;
	World();
	virtual void update(double delta) = 0;
	virtual void initiate(shared_ptr<ShaderGroup> gShader = nullptr) = 0;
	void render();
};

class ExampleWorld : public World {
	void update(double delta) override;
	void initiate(shared_ptr<ShaderGroup> gShader = nullptr) override;
};

class WaterWorld : public World {
	void update(double delta) override;
	void initiate(shared_ptr<ShaderGroup> gShader = nullptr) override;
};

class SponzaWorld : public World {
	void update(double delta) override;
	void initiate(shared_ptr<ShaderGroup> gShader = nullptr) override;
};
