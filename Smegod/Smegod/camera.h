#pragma once
#include "smegod_dep.h"
#include "nodes.h"

class Camera : public WorldObject {
private:

	glm::vec3 position = { 0,0,1 };
	glm::vec3 up = { 0,1.f,0 };
	glm::vec3 front = { 0,0, -1.f };
	glm::vec3 right = glm::cross(up, front);
	glm::mat4 world;

	float min_angle = 0;
	float max_angle = 180;
	const glm::mat4 identity = glm::mat4();

	GLint projection_location;
	GLint view_location;

	float translation_speed;
	float rotation_speed;

public:
	glm::mat4 projection;
	Camera(float fov, int width, int height, float near, float far, GLuint mshader_program);
	void rotate(float deg, glm::vec3 axis);

	void rotateLocalX(float deg);
	void rotateLocalY(float deg);
	void translateLocal(float dx, float dy, float dz);
	glm::mat4& getView();

	void render() override;
	void update(double delta);
};