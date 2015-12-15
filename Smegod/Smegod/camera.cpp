#include "camera.h"
#include "shaders.h"
#include "input_handling.h"

float Camera::getNear() const
{
	return near;
}

float Camera::getFar() const
{
	return far;
}

Camera::Camera(float fov, int width, int height, float near, float far)
{
	projection = glm::perspective(fov, (float)width / (float)height, near, far);

	translation_speed = 10.f; // units/s
	rotation_speed = 90.f; // deg/s
	mouse_sensitivity = 0.3f;
	updateRotation(0,0);
	this->near = near;
	this->far = far;
}

void Camera::translateLocal(float dx, float dy, float dz)
{
	world = glm::translate(world, { dx,dy,dz });
}

void Camera::render(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader, bool renderMaterials)
{
	combined_world = combined_transform;
	view = glm::inverse(combined_transform);
	view_projection = projection * view;
	view_projection_inverse = glm::inverse(view_projection);

	/*for (auto it = shader_groups.begin(); it != shader_groups.end(); ++it) {
		auto program = *it;
		program->use();

		program->setUniform("projection", projection);
		program->setUniform("view", glm::inverse(combined_transform));
		program->setUniform("camera_pos", glm::vec3(combined_transform[3]));
	}*/
}


void Camera::update(double d)
{
	float delta = (float)d;
	handleKeyboard(delta);
	handleMouse(delta);
}

void Camera::handleKeyboard(float delta)
{
	if (delta <= 0)
		return;

	int moveLeft = InputHandler::getKeystate(GLFW_KEY_A) != GLFW_RELEASE ? -1 : 0;
	int moveRight = InputHandler::getKeystate(GLFW_KEY_D) != GLFW_RELEASE ? 1 : 0;
	int moveForward = InputHandler::getKeystate(GLFW_KEY_W) != GLFW_RELEASE ? -1 : 0;
	int moveBackward = InputHandler::getKeystate(GLFW_KEY_S) != GLFW_RELEASE ? 1 : 0;

	float trans_factor = translation_speed * delta;

	int slow = InputHandler::key_mods & GLFW_MOD_SHIFT;
	if (slow == GLFW_MOD_SHIFT) {
		trans_factor *= 0.3f;
	}

	translateLocal((moveLeft + moveRight)*trans_factor, 0, (moveForward + moveBackward)*trans_factor);

	int rotateLeft = InputHandler::getKeystate(GLFW_KEY_LEFT) != GLFW_RELEASE ? -1 : 0;
	int rotateRight = InputHandler::getKeystate(GLFW_KEY_RIGHT) != GLFW_RELEASE ? 1 : 0;
	int rotateUp = InputHandler::getKeystate(GLFW_KEY_UP) != GLFW_RELEASE ? 1 : 0;
	int rotateDown = InputHandler::getKeystate(GLFW_KEY_DOWN) != GLFW_RELEASE ? -1 : 0;

	float rot_factor = rotation_speed * delta;
	updateRotation((rotateLeft + rotateRight)*rot_factor, (rotateUp + rotateDown)*rot_factor);
}

void Camera::updateRotation(float ry, float rx)
{
	yaw += ry;
	pitch -= rx;

	if (pitch > max_angle) {
		pitch = max_angle;
	}
	else if (pitch < min_angle) {
		pitch = min_angle;
	}

	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	front = glm::normalize(front);

	auto right = glm::normalize(glm::cross(front, world_up));
	auto up = glm::normalize(glm::cross(right, front));


	world[2] = glm::vec4(front,0);
	world[0] = glm::vec4(-right, 0);
	world[1] = glm::vec4(up, 0);
}

void Camera::handleMouse(float delta)
{
	auto takingCursorInput = InputHandler::getMouseButtonstate(GLFW_MOUSE_BUTTON_RIGHT) != GLFW_RELEASE;
	if (takingCursorInput != oldTakingCursorInput) {
		oldTakingCursorInput = takingCursorInput;
		glfwSetInputMode(InputHandler::active_window, GLFW_CURSOR, takingCursorInput ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}
	if (takingCursorInput) {
		auto coord = InputHandler::getMouseDelta();
		coord.x *= mouse_sensitivity;
		coord.y *= mouse_sensitivity;
		updateRotation((float) coord.x , (float)coord.y);
	}

}





