#pragma once
#include "gl_dep.h"
#include <iostream>
#include <memory>
#include "world.h"
class InputHandler {
private:
	static unique_ptr<vector<int>> keystate;
public:
	
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static const int getKeystate(int key);
};
