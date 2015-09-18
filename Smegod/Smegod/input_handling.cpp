#include "input_handling.h"

unique_ptr<vector<int>> InputHandler::keystate = make_unique<vector<int>>(GLFW_KEY_LAST, GLFW_RELEASE);

void InputHandler::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_UNKNOWN) {
		std::cout << "Unknown key" << std::endl;
		return;
	}

	(*keystate)[key] = action;

	/*
	shared_ptr<Camera> camera = world->active_camera;

	if (action == GLFW_RELEASE) {

	}

	if (action != GLFW_RELEASE && camera != nullptr) {
		float d = (float)delta;

		float speed = camera->speed;
		float transSpeed = 1.f * speed * d;
		float rotSpeed = 50.f * speed * d;

		switch (key)
		{
		case GLFW_KEY_A:
			camera->translateLocal(-transSpeed, 0, 0);
			break;
		case GLFW_KEY_D:
			camera->translateLocal(transSpeed, 0, 0);
			break;
		case GLFW_KEY_W:
			camera->translateLocal(0, 0, transSpeed);
			break;
		case GLFW_KEY_S:
			camera->translateLocal(0, 0, -transSpeed);
			break;
		case GLFW_KEY_LEFT:
			camera->rotateLocalY(rotSpeed);
			break;
		case GLFW_KEY_RIGHT:
			camera->rotateLocalY(-rotSpeed);
			break;
		case GLFW_KEY_UP:
			camera->rotateLocalX(rotSpeed);
			break;
		case GLFW_KEY_DOWN:
			camera->rotateLocalX(-rotSpeed);
			break;
		default:
			break;
		}
	}*/
}


const int InputHandler::getKeystate(int key)
{
	return (*keystate)[key];
}