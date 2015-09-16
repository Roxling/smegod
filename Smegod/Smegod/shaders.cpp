#include "shaders.h"
#include <fstream>
#include <streambuf>
#include <iostream>
#include "GL\glew.h"

using namespace std;

Shader::Shader(GLenum mtype, string mfile) : type(mtype), file(mfile) {
	ifstream stream(FOLDER + file);
	code = string((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());
}

Shader::~Shader() {
	if (compiled) {
		glDeleteShader(shader);
	}
}

bool Shader::compile() {
	shader = glCreateShader(type);
	const GLchar *glCode = code.c_str();
	glShaderSource(shader, 1, &glCode, NULL);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		const int logSize = 512;
		GLchar log[logSize];
		glGetShaderInfoLog(shader, logSize, NULL, log);
		cout << "Shader failed to compile (file : " << file << " )." << endl << log << endl;
	}
	compiled = success ? true : false;
	return compiled;
}

void Shader::attachTo(GLuint program) {
	if (compiled) {
		glAttachShader(program, shader);
	}
	else {
		cout << "Tried to attach uncompiled shader." << endl;
	}
}
