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

}


const int InputHandler::getKeystate(int key)
{
	return (*keystate)[key];
}