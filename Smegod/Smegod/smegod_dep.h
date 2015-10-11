#pragma once
#include "gl_dep.h"
#include <vector>
#include <memory>
#include <iostream>
#include <string>

using namespace std;

class Globals {
public:
	static const GLuint WIDTH = 1280, HEIGHT = 720;

	static bool DRAW_FPS;

	static void printVec4(glm::vec4 v) {
		cout << v.x << " : " << v.y << " : " << v.z << endl;
	}

	static void printVec3(glm::vec3 v) {
		cout << v.x << " : " << v.y << " : " << v.z << endl;
	}

	static double GetTime() {
		return glfwGetTime();
	}
};
