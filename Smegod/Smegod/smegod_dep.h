#pragma once
#include "gl_dep.h"
#include <vector>
#include <memory>
#include <iostream>
#include <string>

using namespace std;

static void printVec4(glm::vec4 v) {
	cout << v.x << " : " << v.y << " : " << v.z << endl;
}