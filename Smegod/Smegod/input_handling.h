#pragma once
#include "smegod_dep.h"
#include "world.h"

class Coordinate{
public:
	double x;
	double y;
	Coordinate(double nx, double ny) : x(nx), y(ny) {}
	Coordinate operator-(const Coordinate &c) {
		return Coordinate(this->x - c.x, this->y - c.y);
	}
};

class InputHandler {
public:
	static GLFWwindow* active_window;

private: //keyboard callback
	static unique_ptr<vector<int>> keystate;
public:
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static const int getKeystate(int key);

private: //mouse coord callback
	static double oldx;
	static double dx;
	static double oldy;
	static double dy;
public:
	static void mouse_callback(GLFWwindow* window, double x, double y);
	static Coordinate getMouseDelta();
	static Coordinate getMousePos();

private: //mouse button callback
	static unique_ptr<vector<int>> mouse_buttonstate;
public:
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static const int getMouseButtonstate(int button);
};

class CommandHandler {
private:
	static unique_ptr<vector<pair<bool,bool>>> toggle_state;
	static void set_wireframe(bool state);
	static void recompile_shaders();
	static void freeze_time(bool state);
	static void draw_fps(bool state);
public:
	static void print_help();
	static void handle(GLFWwindow* window, int key, int scancode, int action, int mods);
};
