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
private:
	static unique_ptr<vector<int>> keystate;

	static double oldx;
	static double dx;
	static double oldy;
	static double dy;
public:
	
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_callback(GLFWwindow* window, double x, double y);
	static const int getKeystate(int key);
	static Coordinate getMouseDelta();
};
