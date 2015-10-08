#pragma once
#include "smegod_dep.h"
#include "nodes.h"

class Camera : public WorldObject, public ShaderCollection {
public:
	glm::vec3 position = { 0,0,0 };
private:
	//void attach(shared_ptr<Node> child) override {};
protected:
	glm::vec3 up = { 0, 1.f, 0 };
	glm::vec3 world_up = up;
	glm::vec3 front = { 0, 0, 1.f };
	glm::vec3 right = glm::normalize(glm::cross(front, world_up));;


	float yaw = 0;
	float pitch = 0;
	float roll = 0;

	float max_angle = 89.9999f;
	float min_angle = -max_angle;

	const glm::mat4 identity = glm::mat4();
	glm::mat4 view = glm::mat4();

	bool oldTakingCursorInput = false;

	float translation_speed = 10.f;
	float rotation_speed = 90.f;
	float mouse_sensitivity = 45.f;

	virtual void handleMouse(float delta);
	virtual void handleKeyboard(float delta);

	void updateRotation(float ry, float rx);

	void updateView();
public:
	glm::mat4 projection;
	Camera(float fov, int width, int height, float near, float far);
	void translateLocal(float dx, float dy, float dz);

	void update(double delta);

	void render(glm::mat4 combined_transform) override;
};