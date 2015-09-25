#include "shaders.h"
#include <fstream>
#include <streambuf>


using namespace std;

Shader::Shader(GLenum mtype, string mfile) : type(mtype), file(mfile) {
	code = "";
	
}

Shader::~Shader()
{
	if (compiled) {
		glDeleteShader(shader);
	}
}

bool Shader::compile() {
	ifstream stream(FOLDER + file);
	string ncode = string((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());
	stream.close();
	if (code.compare(ncode) == 0) {
		cout << "No changes on " << file << "." << endl;
		return true;
	}
	else {
		cout << "Compiling " << file << "." << endl;
		glDeleteShader(shader);
		code = ncode;
	}
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


unique_ptr<vector<ShaderGroup*>> ShaderGroup::all_groups = make_unique<vector<ShaderGroup*>>();
void ShaderGroup::recompile_all()
{
	for (auto it = all_groups->begin(); it != all_groups->end(); ++it) {
		(*it)->compile();
	}
}
ShaderGroup::ShaderGroup(string vs, string fs)
{
	all_groups->push_back(this);
	vshader = make_unique<VertexShader>(vs);
	fshader = make_unique<FragmentShader>(fs);
	compile();
	
}
void ShaderGroup::compile()
{
	bool vs_status = vshader->compile();
	bool fs_status = fshader->compile();
	if (vs_status && fs_status) {
		shader_program = glCreateProgram();
		vshader->attachTo(shader_program);

		fshader->attachTo(shader_program);
		link();
	}
	else {
		if (!vs_status)
			cout << vshader->file << " failed to compile. Fix it and try again." << endl;
		if(!fs_status)
			cout << fshader->file << " failed to compile. Fix it and try again." << endl;
	}
	
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

