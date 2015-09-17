#include "input_handling.h"

void InputHandler::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	shared_ptr<Camera> camera = world->active_camera;
	if (action != GLFW_RELEASE && camera != nullptr) {
		switch (key)
		{
		case GLFW_KEY_A:
			camera->translateLocal(-0.1f, 0, 0);
			break;
		case GLFW_KEY_D:
			camera->translateLocal(0.1f, 0, 0);
			break;
		case GLFW_KEY_W:
			camera->translateLocal(0, 0, 0.1f);
			break;
		case GLFW_KEY_S:
			camera->translateLocal(0, 0, -0.1f);
			break;
		case GLFW_KEY_LEFT:
			camera->rotateLocalY(5.f);
			break;
		case GLFW_KEY_RIGHT:
			camera->rotateLocalY(-5.f);
			break;
		case GLFW_KEY_UP:
			camera->rotateLocalX(5.f);
			break;
		case GLFW_KEY_DOWN:
			camera->rotateLocalX(-5.f);
			break;
		default:
			break;
		}
	}
}
