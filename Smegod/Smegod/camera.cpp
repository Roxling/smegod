#include "camera.h"
#include "shaders.h"
#include "input_handling.h"

Camera::Camera(float fov, int width, int height, float near, float far) : WorldObject(nullptr)
{
	projection = glm::perspective(fov, (float)width / (float)height, near, far);

	translation_speed = 10.f; // units/s
	rotation_speed = 90.f; // deg/s
}

void Camera::addShaderGroup(shared_ptr<ShaderGroup> sg)
{
	shader_groups.push_back(sg);
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
	vec = glm::length(vec) > 0 ? vec : side;
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
	side = rot * side;
}

void Camera::updateView()
{
	world = glm::lookAtRH(position, position + front, up);
}

void Camera::renderSelf(glm::mat4 combined_transform)
{

	GLint projection_location;
	GLint view_location;
	for (auto it = shader_groups.begin(); it != shader_groups.end(); ++it) {
		auto program = (*it)->getProgram();
		(*it)->use();
		projection_location = glGetUniformLocation(program, "projection");
		view_location = glGetUniformLocation(program, "view");
		glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(combined_transform));
	}
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

	//when all translations and rotations are done, update world matrix.
	updateView();
}



