#pragma once
#include "smegod_dep.h"
#include "world.h"

class Coordinate{
public:
	double x;
	double y;
	Coordinate(double nx, double ny) : x(nx), y(ny) {}
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

private: //mouse button callback
	static unique_ptr<vector<int>> mouse_buttonstate;
public:
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static const int getMouseButtonstate(int button);
};
