#include "camera.h"
#include "shaders.h"
#include "input_handling.h"

Camera::Camera(float fov, int width, int height, float near, float far, shared_ptr<ShaderGroup> mshader_group) : WorldObject(mshader_group)
{
	projection = glm::perspective(fov, (float)width / (float)height, near, far);
	projection_location = glGetUniformLocation(shader_group->getProgram(), "projection");
	view_location = glGetUniformLocation(shader_group->getProgram(), "view");

	translation_speed = 10.f; // units/s
	rotation_speed = 90.f; // deg/s
}

void Camera::translateLocal(float dx, float dy, float dz)
{
	position = position + dz*glm::normalize(front) +
						  dy*glm::normalize(up) +
						  dx*glm::normalize(glm::cross(front,up));
}

float eps = 0.1f;
void Camera::rotateLocalX(float deg)
{
	auto ang = glm::degrees(glm::acos(glm::dot(up, front)));
	if (ang + deg < min_angle){
		deg = -ang + eps;
		return;
	}
	else if (ang + deg > max_angle) {
		deg = max_angle - ang - eps;
		return;
	}
	auto vec = glm::cross(up, front);
	vec = glm::length(vec) > 0 ? vec : right;
	rotate(deg, vec);

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
	right = rot * right;
}

glm::mat4 & Camera::getView()
{
	world = glm::lookAt(position, position + front, up);
	return world;
}

void Camera::render()
{
	glUseProgram(shader_group->getProgram());
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


