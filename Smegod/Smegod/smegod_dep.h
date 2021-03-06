#pragma once
#include "gl_dep.h"
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

//#define RUN_FULLSCREEN

class Globals {
public:
#ifdef RUN_FULLSCREEN
	static const GLsizei WIDTH = 1920, HEIGHT = 1080;
	static const bool FULLSCREEN = true;
#else
	static const GLsizei WIDTH = 1280, HEIGHT = 720;
	static const bool FULLSCREEN = false;
#endif
	static const GLsizei SHADOW_WIDTH = 720, SHADOW_HEIGHT = 480;

	static bool WIREFRAME;

	static bool DRAW_FPS;
	static bool DRAW_BUFFERS;
	static bool UNIFORM_REFRESH;
	static bool TIME_NOT_FROZEN;

	static void printVec4(glm::vec4 v) {
		cout << v.x << " : " << v.y << " : " << v.z << endl;
	}

	static void printVec3(glm::vec3 v) {
		cout << v.x << " : " << v.y << " : " << v.z << endl;
	}

	static double GetTime() {
		return glfwGetTime();
	}

	static bool File_Exists(const string fileName)
	{
		ifstream f(fileName.c_str());
		return f.good();
	}
};

#undef RUN_FULLSCREEN
