#pragma once
#include "smegod_dep.h"
#include <SOIL.h>

class Cubemap {
private:
	const string FOLDER = "Textures/";
public:
	Cubemap() {}
	GLuint texture_id = 0;
	Cubemap(string file);
	static shared_ptr<Cubemap> DEFAULT;
};