#pragma once
#include "smegod_dep.h"
#include "SOIL.h"

class Texture {
private:
	const string FOLDER = "Textures/";

public:
	GLuint texture_id;
	Texture(string file);
};