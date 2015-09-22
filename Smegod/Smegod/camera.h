#pragma once
#include "smegod_dep.h"
#include "nodes.h"

class Camera : public WorldObject, public ShaderCollection {
private:
	glm::vec3 position = { 0,0,0 };
	glm::vec3 up = { 0, 1.f, 0 };
	glm::vec3 world_up = up;
	glm::vec3 front = { 0, 0, 1.f };
	glm::vec3 right;


	float yaw;
	float pitch;

	float min_angle = -89.9f;
	float max_angle = 89.9f;

	const glm::mat4 identity = glm::mat4();

	float translation_speed;
	float rotation_speed;
	float mouse_sensitivity;

	void handleMouse(float delta);
	void handleKeyboard(float delta);

	void updateRotation(float ry, float rx);

	void updateView();
public:
	glm::mat4 projection;
	Camera(float fov, int width, int height, float near, float far);
	void translateLocal(float dx, float dy, float dz);

	void render() override {}
	void update(double delta);

	void renderSelf(glm::mat4 combined_transform) override;
};