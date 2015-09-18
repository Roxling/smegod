#pragma once
#include "gl_dep.h"
#include <memory>
#include "world.h"
class InputHandler {
private:
	shared_ptr<World> world;
public:
	InputHandler(shared_ptr<World> mworld) : world(mworld) {}
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods, double delta);
};
