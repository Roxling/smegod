#include "input_handling.h"
#include "shaders.h"

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
	CommandHandler::handle(window, key, scancode, action, mods);
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

Coordinate InputHandler::getMousePos()
{
	return Coordinate(oldx, oldy);
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

unique_ptr<vector<pair<bool, bool>>> CommandHandler::toggle_state = make_unique<vector<pair<bool,bool>>>(GLFW_KEY_LAST, make_pair(true,true)); //{first press callback, new state on toggle}

bool Globals::WIREFRAME = false;
void CommandHandler::set_wireframe(bool state)
{
	cout << "Wirefram toggled " <<(state ? "ON" : "OFF") << "." << endl;
	Globals::WIREFRAME = state;
	 //Uncomment for wireframe
}

void CommandHandler::recompile_shaders()
{
	cout << "Recompiling shaders." << endl;
	ShaderGroup::recompile_all();
}

bool Globals::DRAW_FPS = false;
void CommandHandler::draw_fps(bool state)
{
	Globals::DRAW_FPS = state;
	cout << "FPS toggled " << (state ? "ON" : "OFF") << "." << endl;
}

void CommandHandler::print_help()
{
	string help = 
		"\t\t --HELP-- \n"
		"Key controls: \n"
		"'W,A,S,D' to translate camera. \n"
		"'Arrow keys' or 'right click + mouse' to rotate camera. \n"
		"'1' to toggle wireframe. (Default OFF)\n"
		"'H' to print help text (this). \n"
		"'R' to recompile shaders. \n"
		"'F' to toggle FPS counter. (Default OFF) \n"
		"\t\t --HELP-- \n";
	cout << help;
}

void CommandHandler::handle(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS && (*toggle_state)[key].first) {

		switch (key)
		{
		case GLFW_KEY_1:
			set_wireframe((*toggle_state)[key].second);
			break;
		case GLFW_KEY_F:
			draw_fps((*toggle_state)[key].second);
			break;
		case GLFW_KEY_H:
			print_help();
			break;
		case GLFW_KEY_R:
			recompile_shaders();
		default:
			break;
		}
		(*toggle_state)[key].second = !(*toggle_state)[key].second;
	}

	(*toggle_state)[key].first = (action == GLFW_RELEASE);
}
