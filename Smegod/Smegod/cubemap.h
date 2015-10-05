#pragma once
#include "smegod_dep.h"
#include <SOIL.h>

class Cubemap {
private:
	const string FOLDER = "Textures/";
public:
	GLuint texture_id;
	Cubemap(string prefix, string suffix);
};