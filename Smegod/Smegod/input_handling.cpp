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

double InputHandler::mouse_x;
double InputHandler::mouse_dx;
double InputHandler::mouse_y;
double InputHandler::mouse_dy;

Coordinate InputHandler::getMouseDelta()
{
	Coordinate c(mouse_dx, mouse_dy);
	mouse_dx = 0;
	mouse_dy = 0;
	return c;
}

void InputHandler::mouse_callback(GLFWwindow * window, double x, double y)
{
	static bool first = true;
	if (first) {
		first = false;
		mouse_x = x;
		mouse_y = y;
	}

	mouse_dx += x - mouse_x;
	mouse_dy += mouse_y - y;
}
