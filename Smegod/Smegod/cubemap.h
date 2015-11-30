#pragma once
#include "smegod_dep.h"
#include <SOIL.h>

class Cubemap {
private:
	const string FOLDER = "Textures/";
	Cubemap(vector<string> imgs);
public:
	GLuint texture_id;
	Cubemap(string prefix, string suffix);
	Cubemap(string prefix, string tb, string side);
};