#pragma once
#include "core.h"
#include "smegod_dep.h"
#include "materials.h"
#include "textures.h"
#include "cubemap.h"

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

class GeometryShader : public Shader {
private:
public:
	vector<GLchar *> varyings;
	GeometryShader(string file) : Shader(GL_GEOMETRY_SHADER, file) {}
	GeometryShader(string file, vector<GLchar*> varyings) : Shader(GL_GEOMETRY_SHADER, file) {
		this->varyings = varyings;
	}

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
	unique_ptr<GeometryShader> gshader;
	bool link();


	//cached locations
	unordered_map<string, ShaderLocation> attributeLocs;
	unordered_map<string, ShaderLocation> uniformLocs;

	bool getUniformLocation(GLuint &dst, const string &name);

public:
	static void recompile_all();
	static void checkUniforms();
	ShaderGroup(const string &vs, const string &fs);
	ShaderGroup(const string &vs, const string &gs, const string &fs);
	ShaderGroup(const string &vs, const string &gs, const string &fs, vector<GLchar*> varyings);

	void compile();
	void use() { 
		glUseProgram(glId);
		GL_CHECK_ERRORS_MSG("Failed to use "+ vshader->file);
	}
	void bindMaterial(shared_ptr<Material> material);

	void debugUnsetUniforms();

	bool bindTexture(const string &name, const unsigned int slot, shared_ptr<Texture> t);
	bool bindCubemap(const string &name, const unsigned int slot, Cubemap &c);
	template<typename T> bool setUniform(const string &name, const T &value);

};
