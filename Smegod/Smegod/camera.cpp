#include "camera.h"
#include <iostream>
#include "shaders.h"
#include "input_handling.h"

Camera::Camera(float fov, int width, int height, float near, float far, GLuint mshader_program)
{
	projection = glm::perspective(fov, (float)width / (float)height, near, far);
	shader_program = mshader_program;
	projection_location = glGetUniformLocation(shader_program, "projection");
	view_location = glGetUniformLocation(shader_program, "view");

	translation_speed = 10.f; // units/s
	rotation_speed = 90.f; // deg/s
}

void Camera::translateLocal(float dx, float dy, float dz)
{
	position = position + dz*glm::normalize(front) +
						  dy*glm::normalize(up) +
						  dx*glm::normalize(glm::cross(front,up));
}

void Camera::rotateLocalX(float deg)
{
	auto vec = glm::cross(up,front);
	if (glm::length(vec) > 0) {

		rotate(deg, vec);
	}

}
void Camera::rotateLocalY(float deg)
{
	rotate(deg, up);
}

void Camera::rotate(float deg, glm::vec3 axis)
{
	//somethings fishy here!
	glm::mat3 rot = glm::mat3(glm::rotate(identity, glm::radians(deg), axis));
	front = rot * front;
	//up = rot * up;
}

glm::mat4 & Camera::getView()
{
	world = glm::lookAt(position, position + front, up);
	return world;
}

void Camera::render()
{
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(getView()));
}

void Camera::update(double d)
{
	float delta = (float) d;
	int moveLeft = InputHandler::getKeystate(GLFW_KEY_A) != GLFW_RELEASE ? -1 : 0;
	int moveRight = InputHandler::getKeystate(GLFW_KEY_D) != GLFW_RELEASE ? 1 : 0;
	int moveForward = InputHandler::getKeystate(GLFW_KEY_W) != GLFW_RELEASE ? 1 : 0;
	int moveBackward = InputHandler::getKeystate(GLFW_KEY_S) != GLFW_RELEASE ? -1 : 0;

	float trans_factor = translation_speed * delta;

	translateLocal((moveLeft + moveRight)*trans_factor, 0, (moveForward + moveBackward)*trans_factor);

	int rotateLeft = InputHandler::getKeystate(GLFW_KEY_LEFT) != GLFW_RELEASE ? 1 : 0;
	int rotateRight = InputHandler::getKeystate(GLFW_KEY_RIGHT) != GLFW_RELEASE ? -1 : 0;
	int rotateUp = InputHandler::getKeystate(GLFW_KEY_UP) != GLFW_RELEASE ? -1 : 0;
	int rotateDown = InputHandler::getKeystate(GLFW_KEY_DOWN) != GLFW_RELEASE ? 1 : 0;

	float rot_factor = rotation_speed * delta;

	rotateLocalY((rotateLeft + rotateRight)*rot_factor);
	rotateLocalX((rotateUp + rotateDown)*rot_factor);
}


