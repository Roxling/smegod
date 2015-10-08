#include "camera.h"
#include "shaders.h"
#include "input_handling.h"

Camera::Camera(float fov, int width, int height, float near, float far) : WorldObject(nullptr)
{
	projection = glm::perspective(fov, (float)width / (float)height, near, far);

	translation_speed = 200.f; // units/s
	rotation_speed = 90.f; // deg/s
	mouse_sensitivity = 0.3f;
	updateRotation(0,0);
}

void Camera::translateLocal(float dx, float dy, float dz)
{
	/*position = position + dz*glm::normalize(front) +
						  dy*glm::normalize(up) +
						  dx*glm::normalize(right);*/

	world = glm::translate(world, { dx,dy,dz });
}

void Camera::updateView()
{
	
	//world = glm::inverse(view);

	auto rot = glm::mat4(glm::quat({ glm::radians(-pitch),{ glm::radians(yaw) },glm::radians(-roll) }));
	world = world * rot;
}

void Camera::render(glm::mat4 combined_transform)
{

	GLint projection_location;
	GLint view_location;
	GLuint camera_pos_location;

	for (auto it = shader_groups.begin(); it != shader_groups.end(); ++it) {
		auto program = (*it)->getProgram();
		(*it)->use();
		projection_location = glGetUniformLocation(program, "projection");
		view_location = glGetUniformLocation(program, "view");
		glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(glm::inverse(combined_transform)));

		camera_pos_location = glGetUniformLocation(program, "camera_pos");
		glUniform3fv(camera_pos_location, 1, glm::value_ptr(glm::vec3(combined_transform[3])));
	}
}


void Camera::update(double d)
{
	float delta = (float)d;
	handleKeyboard(delta);
	handleMouse(delta);

	//when all translations and rotations are done, update world matrix.
	updateView();
}

void Camera::handleKeyboard(float delta)
{

	int moveLeft = InputHandler::getKeystate(GLFW_KEY_A) != GLFW_RELEASE ? -1 : 0;
	int moveRight = InputHandler::getKeystate(GLFW_KEY_D) != GLFW_RELEASE ? 1 : 0;
	int moveForward = InputHandler::getKeystate(GLFW_KEY_W) != GLFW_RELEASE ? 1 : 0;
	int moveBackward = InputHandler::getKeystate(GLFW_KEY_S) != GLFW_RELEASE ? -1 : 0;

	float trans_factor = translation_speed * delta;

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
	pitch += rx;
	
	if (pitch > max_angle) {
		pitch = max_angle;
	}
	else if (pitch < min_angle) {
		pitch = min_angle;
	}

	glm::vec3 nfront;
	nfront.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	nfront.y = sin(glm::radians(pitch));
	nfront.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	front = glm::normalize(nfront);

	right = glm::normalize(glm::cross(front, world_up));
	up = glm::normalize(glm::cross(right, front));
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





