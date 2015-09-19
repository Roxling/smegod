#pragma once
#include "smegod_dep.h"

using namespace std;

class Shader abstract {
private:
	const string FOLDER = "Shaders/";

	GLenum type;
	string code;
	string file;

	GLuint shader;
	bool compiled;

	GLuint program;
protected:
	Shader(GLenum mtype, string mfile);
	~Shader();

public:
	bool compile();
	GLuint getProgram();
	void attachTo(GLuint program);
};

class VertexShader : public Shader {
public:
	VertexShader(string file) : Shader(GL_VERTEX_SHADER, file) {}
};

class PixelShader : public Shader {
public:
	PixelShader(string file) : Shader(GL_FRAGMENT_SHADER, file) {}
};