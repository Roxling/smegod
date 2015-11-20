#pragma once
#include "smegod_dep.h"
#include "materials.h"

using namespace std;

class Shader abstract {
private:
	const string FOLDER = "Shaders/";

	GLenum type;
	string code = "";

	GLuint shader = 0;
	bool compiled = false;

	GLuint program = 0;
protected:
	Shader(GLenum mtype, string mfile);

public:
	enum COMPILE_STATUS {
		FAILED = -1,
		SUCCESS = 0,
		UNCHANGED = 1
	};
	~Shader();
	string file = nullptr;
	COMPILE_STATUS compile();
	GLuint getProgram();
	void attachTo(GLuint program);
	void detach();
};

class VertexShader : public Shader {
public:
	VertexShader(string file) : Shader(GL_VERTEX_SHADER, file) {}
};

class FragmentShader : public Shader {
public:
	FragmentShader(string file) : Shader(GL_FRAGMENT_SHADER, file) {}
};

class ShaderGroup {
private:
	static unique_ptr<vector<ShaderGroup*>> all_groups;
	GLuint shader_program = 0;
	unique_ptr<VertexShader> vshader;
	unique_ptr<FragmentShader> fshader;
	bool link();
public:
	static void recompile_all();
	ShaderGroup(string vs, string fs);
	GLuint getProgram() { return shader_program; }
	void compile();
	void use() { glUseProgram(shader_program); }
	void bindMaterial(shared_ptr<Material> material);
};