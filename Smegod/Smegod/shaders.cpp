#include "shaders.h"
#include <fstream>
#include <streambuf>


using namespace std;

Shader::Shader(GLenum mtype, string mfile) : type(mtype), file(mfile) {
	ifstream stream(FOLDER + file);
	code = string((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());
	stream.close();
}

Shader::~Shader()
{
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

void Shader::attachTo(GLuint nprogram) {
	if (compiled) {
		glAttachShader(nprogram, shader);
		program = nprogram;
	}
	else {
		cout << "Tried to attach uncompiled shader." << endl;
	}
}



GLuint Shader::getProgram()
{
	return program;
}

ShaderGroup::ShaderGroup(string vertexfile, string pixelfile)
{
	shader_program = glCreateProgram();

	VertexShader vshader(vertexfile);
	vshader.compile();
	vshader.attachTo(shader_program);

	PixelShader pshader(pixelfile);
	pshader.compile();
	pshader.attachTo(shader_program);

	link();
}
void ShaderGroup::link()
{
	glLinkProgram(shader_program);

	GLint success;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		const int logSize = 512;
		GLchar log[logSize];

		glGetProgramInfoLog(shader_program, logSize, NULL, log);
		cout << "Shader program failed to link." << endl << log << endl;
	}

}

