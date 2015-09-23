#include "input_handling.h"

GLFWwindow* InputHandler::active_window = nullptr;

//keyboard button callback
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


//mouse screen x,y callback
double InputHandler::oldx = 0;
double InputHandler::dx = 0;
double InputHandler::oldy = 0;
double InputHandler::dy = 0;

Coordinate InputHandler::getMouseDelta()
{
	Coordinate c(dx, dy);
	dx = 0;
	dy = 0;
	return c;
}

void InputHandler::mouse_callback(GLFWwindow * window, double x, double y)
{
	static bool first = true;
	if (first) {
		first = false;
		oldx = x;
		oldy = y;
	}

	dx = x - oldx;
	dy = oldy - y;

	oldx = x;
	oldy = y;
}

//mouse button callback
unique_ptr<vector<int>> InputHandler::mouse_buttonstate = make_unique<vector<int>>(GLFW_MOUSE_BUTTON_LAST, GLFW_RELEASE);

void InputHandler::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	(*mouse_buttonstate)[button] = action;
}

const int InputHandler::getMouseButtonstate(int button)
{
	return (*mouse_buttonstate)[button];
}
