#pragma once
#include "gl_dep.h"

class Camera {
private:
	glm::vec3 position = { 0,0,1 };
	glm::vec3 up = { 0,1.f,0 };
	glm::vec3 front = { 0,0, -1.f };
	glm::mat4 world;
	const glm::mat4 identity = glm::mat4();

	GLuint shader_program;

public:
	glm::mat4 projection;
	Camera(float fov, int width, int height, float near, float far);
	void rotate(float deg, glm::vec3 axis);

	void rotateLocalX(float deg);
	void rotateLocalY(float deg);
	void translateLocal(float dx, float dy, float dz);
	glm::mat4& getView();

	void setupShader(GLuint shader_program);
	void render();
};