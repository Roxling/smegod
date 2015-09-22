#pragma once
#include "smegod_dep.h"
#include "nodes.h"

class Camera : public WorldObject, ShaderCollection {
private:
	glm::vec3 position = { 0,0,0 };
	glm::vec3 up = { 0, 1.f, 0 };
	glm::vec3 front = { 0, 0, 1.f };
	glm::vec3 side = glm::cross(front, up);


	float min_angle = 0;
	float max_angle = 180;
	const glm::mat4 identity = glm::mat4();

	float translation_speed;
	float rotation_speed;

public:
	glm::mat4 projection;
	Camera(float fov, int width, int height, float near, float far);
	void rotate(float deg, glm::vec3 axis);
	void rotateLocalX(float deg);
	void rotateLocalY(float deg);
	void translateLocal(float dx, float dy, float dz);
	void updateView();

	void render() override {}
	void update(double delta);

	void renderSelf(glm::mat4 combined_transform) override;
};