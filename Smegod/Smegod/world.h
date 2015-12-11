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
	virtual void initiate() = 0;
	void render(shared_ptr<ShaderGroup> gShader, bool renderMaterials = true);
};

class ExampleWorld : public World {
	void update(double delta) override;
	void initiate() override;
};

class WaterWorld : public World {
	void update(double delta) override;
	void initiate() override;
};

class SponzaWorld : public World {
	void update(double delta) override;
	void initiate() override;
};
