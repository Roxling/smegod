#pragma once
#include "smegod_dep.h"
#include <SOIL.h>
#include <unordered_map>

class DefaultTextures {
public:
	DefaultTextures(GLuint t, GLuint b) : texture(t), bump(b) {}
	GLuint texture;
	GLuint bump;
};

class Texture {
private:
	static shared_ptr<DefaultTextures> defaultInstance;

	static unique_ptr<unordered_map<string, GLuint>> cache;
	const string FOLDER = "Textures/";
public:
	Texture(){}
	GLuint texture_id = 0;
	Texture(string file, bool use_defaultfolder = true);
	static shared_ptr<DefaultTextures> getDefaults();
};