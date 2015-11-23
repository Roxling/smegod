#pragma once
#include "core.h"
#include "smegod_dep.h"
#include "materials.h"
#include "textures.h"

using namespace std;

class Shader abstract : public GLResource {
private:
	const string FOLDER = "Shaders/";

	GLenum type;
	string code = "";

	bool compiled = false;

protected:
	Shader(GLenum mtype, string mfile);

public:
	enum CompileStatus {
		FAILED = -1,
		SUCCESS = 0,
		UNCHANGED = 1
	};
	~Shader();
	string file = nullptr;
	CompileStatus compile();
	void attachTo(GLuint program);
	void detachFrom(GLuint program);
};

class VertexShader : public Shader {
public:
	VertexShader(string file) : Shader(GL_VERTEX_SHADER, file) {}
};

class FragmentShader : public Shader {
public:
	FragmentShader(string file) : Shader(GL_FRAGMENT_SHADER, file) {}
};

struct ShaderLocation {
	string name;
	GLuint loc;
	GLint size;
	GLenum type;
	bool isSet;
};

class ShaderGroup : public GLResource {
private:
	static unique_ptr<vector<ShaderGroup*>> all_groups;
	unique_ptr<VertexShader> vshader;
	unique_ptr<FragmentShader> fshader;
	bool link();

	//cached locations
	unordered_map<string, ShaderLocation> attributeLocs;
	unordered_map<string, ShaderLocation> uniformLocs;

	bool getUniformLocation(GLuint &dst, const string &name);

public:
	static void recompile_all();
	ShaderGroup(const string &vs, const string &fs);
	void compile();
	void use() { 
		glUseProgram(glId);
		GL_CHECK_ERRORS();
	}
	void bindMaterial(shared_ptr<Material> material);


	bool bindTexture(const string &name, const unsigned int slot, Texture &t);
	template<typename T> bool setUniform(const string &name, const T &value);

};
