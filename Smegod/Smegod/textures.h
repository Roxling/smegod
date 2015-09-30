#pragma once
#include "smegod_dep.h"
#include <SOIL.h>

class Texture {
private:
	const string FOLDER = "Textures/";
public:
	Texture(){}
	GLuint texture_id = 0;
	Texture(string file);
	static shared_ptr<Texture> DEFAULT;
};