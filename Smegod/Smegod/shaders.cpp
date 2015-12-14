#include "shaders.h"
#include "cubemap.h"
#include <fstream>
#include <streambuf>

using namespace std;

#define ERR_BUF_LEN (8192)

#if DEBUG_LEVEL >= 2
#	define GL_CHECK_SHADER_BOUND(id)		check_bound_shader_program(id, __FILE__, __FUNCTION__, __LINE__)
#else
#	define GL_CHECK_SHADER_BOUND(sp)
#endif

static void check_bound_shader_program(GLuint id, const char *file, const char *function, int line)
{
	GLint curr;
	glGetIntegerv(GL_CURRENT_PROGRAM, &curr);
	if (id != curr) {
		cout << file << ":" << function << "(" << line << "). You must bind the shader program before calling this function" << endl;
	}
}

Shader::Shader(GLenum mtype, string mfile) : type(mtype), file(mfile) {}

Shader::~Shader()
{
	glDeleteShader(glId);
	GL_CHECK_ERRORS();
}

Shader::CompileStatus Shader::compile() {

	ifstream stream(FOLDER + file);
	if (!stream.good()) {
		cout << "File " << FOLDER + file << " does not exist. Can not load (or compile) shader." << endl;
		return CompileStatus::FAILED;
	}
	string ncode = string((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());
	stream.close();
	if (code.compare(ncode) == 0) {
		return CompileStatus::UNCHANGED;
	}
	else {
		cout << "Compiling " << file << "." << endl;
		if (compiled) {
			glDeleteShader(glId);
			GL_CHECK_ERRORS();
		}
	}
	glId = glCreateShader(type);
	GL_CHECK_ERRORS();
	const GLchar *glCode = ncode.c_str();
	glShaderSource(glId, 1, &glCode, NULL);
	GL_CHECK_ERRORS();
	glCompileShader(glId);
	GL_CHECK_ERRORS();

	GLint success;
	glGetShaderiv(glId, GL_COMPILE_STATUS, &success);
	GL_CHECK_ERRORS();
	GLchar result[ERR_BUF_LEN];
	GLsizei length;
	if (success == GL_FALSE) {
		glGetShaderInfoLog(glId, ERR_BUF_LEN, &length, result);
		glDeleteShader(glId);
		GL_CHECK_ERRORS();
		if (length) {
			cout << result << endl;
		}
		cout << "Shader failed to compile (file : " << file << " )." << endl;
	}
	else {
		glGetShaderInfoLog(glId, ERR_BUF_LEN, &length, result);
		if (length) {
			cout << "Shader compiler message for " << file << ": " << result << endl;
		}
		code = ncode;
	}
	compiled = success == GL_TRUE ? true : false;
	return compiled ? CompileStatus::SUCCESS : CompileStatus::FAILED;
}

void Shader::attachTo(GLuint nprogram) {
	if (compiled) {
		glAttachShader(nprogram, glId);
		GL_CHECK_ERRORS();
	}
	else {
		cout << "Tried to attach uncompiled shader." << endl;
	}
}

void Shader::detachFrom(GLuint nprogram)
{
	glDetachShader(nprogram, glId);
	GL_CHECK_ERRORS();
}

unique_ptr<vector<ShaderGroup*>> ShaderGroup::all_groups = make_unique<vector<ShaderGroup*>>();
void ShaderGroup::recompile_all()
{
	for (auto it = all_groups->begin(); it != all_groups->end(); ++it) {
		(*it)->compile();
	}
}

ShaderGroup::ShaderGroup(const string &vs, const string &fs)
{
	glId = glCreateProgram();
	GL_CHECK_ERRORS();
	all_groups->push_back(this);
	vshader = make_unique<VertexShader>(vs);
	gshader = nullptr;
	fshader = make_unique<FragmentShader>(fs);
	compile();
}

ShaderGroup::ShaderGroup(const string &vs, const string &gs, const string &fs)
{
	glId = glCreateProgram();
	GL_CHECK_ERRORS();
	all_groups->push_back(this);
	vshader = make_unique<VertexShader>(vs);
	gshader = make_unique<GeometryShader>(gs);
	fshader = make_unique<FragmentShader>(fs);
	compile();
}

ShaderGroup::ShaderGroup(const string &vs, const string &gs, const string &fs, vector<GLchar*> varyings)
{
	glId = glCreateProgram();
	GL_CHECK_ERRORS();
	all_groups->push_back(this);
	vshader = make_unique<VertexShader>(vs);
	gshader = make_unique<GeometryShader>(gs, varyings);
	fshader = make_unique<FragmentShader>(fs);
	compile();
}

void ShaderGroup::compile()
{
	auto vs_status = vshader->compile();
	auto fs_status = fshader->compile();
	auto gs_status = Shader::CompileStatus::UNCHANGED;

	if (gshader) {
		gs_status = gshader->compile();
	}


	if (vs_status == Shader::CompileStatus::UNCHANGED && fs_status == Shader::CompileStatus::UNCHANGED && gs_status == Shader::CompileStatus::UNCHANGED) {
		cout << "No changes in " << vshader->file << " or linked shaders." << endl;
		return;
	}

	if (vs_status != Shader::CompileStatus::FAILED && fs_status != Shader::CompileStatus::FAILED && gs_status != Shader::CompileStatus::FAILED) {
		
		vshader->attachTo(glId);
		fshader->attachTo(glId);
		if (gshader) {
			gshader->attachTo(glId);
			if (gshader->varyings.size() > 0) {
				glTransformFeedbackVaryings(glId,(GLsizei) gshader->varyings.size() , gshader->varyings.data(), GL_INTERLEAVED_ATTRIBS);
			}
		}
		if (link()) {
			vshader->detachFrom(glId);
			fshader->detachFrom(glId);
			if (gshader) {
				gshader->detachFrom(glId);
			}
		}
		else {
			//glDeleteProgram(shader_program);
		}
	}
	else{
		if (vs_status == Shader::CompileStatus::FAILED)
			cout << vshader->file << " failed to compile. Fix it and try again." << endl;
		if(fs_status == Shader::CompileStatus::FAILED)
			cout << fshader->file << " failed to compile. Fix it and try again." << endl;
		if (gshader && gs_status == Shader::CompileStatus::FAILED)
			cout << gshader->file << " failed to compile. Fix it and try again." << endl;
	}

	//Create changed location entries
	attributeLocs.clear();
	uniformLocs.clear();

	char buf[1024];
	GLsizei len;
	GLint size;
	GLenum type;

	// Create attribute map
	GLint nAttribs;
	glGetProgramiv(glId, GL_ACTIVE_ATTRIBUTES, &nAttribs);
	GL_CHECK_ERRORS();
	for (int i = 0; i < nAttribs; i++) {
		glGetActiveAttrib(glId, i, 1023, &len, &size, &type, buf);
		GL_CHECK_ERRORS();
		ShaderLocation atr;
		atr.name = string(buf);
		atr.loc = glGetAttribLocation(glId, buf);
		GL_CHECK_ERRORS();
		atr.size = size; // TODO: Currently unused
		atr.type = type;
		atr.isSet = false;
		attributeLocs[atr.name] = atr;
	}

	// Create uniform map
	GLint nUniforms;
	glGetProgramiv(glId, GL_ACTIVE_UNIFORMS, &nUniforms);
	GL_CHECK_ERRORS();
	for (int i = 0; i < nUniforms; i++) {
		glGetActiveUniform(glId, i, 1023, &len, &size, &type, buf);
		GL_CHECK_ERRORS();
		ShaderLocation un;
		un.name = string(buf);
		un.loc = glGetUniformLocation(glId, buf);
		GL_CHECK_ERRORS();
		un.size = size; // TODO: Currently unused
		un.type = type;
		un.isSet = false;
		uniformLocs[un.name] = un;
	}
}

void ShaderGroup::bindMaterial(shared_ptr<Material> material)
{
	GL_CHECK_SHADER_BOUND(glId);
	for (int i = 0; i < material->textures.size(); ++i) {
		bindTexture(material->textures[i].first, i, material->textures[i].second);
	}
}

bool ShaderGroup::bindTexture(const string &name, const unsigned int slot, shared_ptr<Texture> t)
{
	GL_CHECK_SHADER_BOUND(glId);
	auto match = uniformLocs.find(name);
	if (match == uniformLocs.end()) {
		cout << "Sampler " << name << " not found in shader " << vshader->file << endl; \
		return false;
	}
	match->second.isSet = true;
	glActiveTexture(GL_TEXTURE0 + slot);
	GL_CHECK_ERRORS();
	glBindTexture(t->getTarget(), t->getGlId());
	GL_CHECK_ERRORS();
	glUniform1i(match->second.loc, slot);
	GL_CHECK_ERRORS();

	return true;
}

bool ShaderGroup::bindCubemap(const string & name, const unsigned int slot,Cubemap &c)
{
	GL_CHECK_SHADER_BOUND(glId);
	auto match = uniformLocs.find(name);
	if (match == uniformLocs.end()) {
		cout << "Sampler " << name << " not found in shader " << vshader->file << endl; \
			return false;
	}
	match->second.isSet = true;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, c.texture_id);
	glUniform1i(match->second.loc, slot);
	GL_CHECK_ERRORS();
	return true;
}

bool ShaderGroup::link()
{
	glLinkProgram(glId);
	GL_CHECK_ERRORS();
	GLint success;
	glGetProgramiv(glId, GL_LINK_STATUS, &success);
	GL_CHECK_ERRORS();
	if (!success) {
		GLchar log[ERR_BUF_LEN];
		glGetProgramInfoLog(glId, ERR_BUF_LEN, NULL, log);
		GL_CHECK_ERRORS();
		cout << "Shader program failed to link." << endl << log << endl;
		return false;
	}
	return true;
}

void ShaderGroup::checkUniforms()
{
	for (auto it = all_groups->begin(); it != all_groups->end(); ++it) {
		(*it)->debugUnsetUniforms();
	}
}


void ShaderGroup::debugUnsetUniforms()
{
	bool allSet = true;
	for (auto it = uniformLocs.begin(); it != uniformLocs.end(); ++it) {
		if (!it->second.isSet) {
			cout << "Uniform " << it->second.name.c_str() << " for shader program id " << vshader->file << " does not appear to be set" << endl;
			allSet = false;
		}
	}
}

bool ShaderGroup::getUniformLocation(GLuint &dst, const string &name) {
	auto match = uniformLocs.find(name);
	if (match == uniformLocs.end()) {
		return false;
	}
	// TODO: Add type and size checks
	match->second.isSet = true;
	dst = match->second.loc;
	return true;
}

#define UNIFORM_UPLOAD_SETUP() \
	GL_CHECK_SHADER_BOUND(glId); \
	GLuint loc; \
	if (!getUniformLocation(loc, name) && DEBUG_LEVEL) { \
		cout << "Uniform " << name << " not found in shader " << vshader->file << endl; \
		return false; \
	}


#define UNIFORM_UPLOAD_1(t, f) \
	template<> bool ShaderGroup::setUniform<t>(const std::string &name, const t &v) { \
		UNIFORM_UPLOAD_SETUP() \
		f(loc, v); \
		GL_CHECK_ERRORS_MSG(name); return true; }

#define UNIFORM_UPLOAD_GLMv(t, f) \
	template<> bool ShaderGroup::setUniform<t>(const std::string &name, const t &v) { \
		UNIFORM_UPLOAD_SETUP() \
		f(loc, 1, glm::value_ptr(v)); \
		GL_CHECK_ERRORS_MSG(name); return true; }

#define UNIFORM_UPLOAD_GLMm(t, f) \
	template<> bool ShaderGroup::setUniform<t>(const std::string &name, const t &v) { \
		UNIFORM_UPLOAD_SETUP() \
		f(loc, (GLsizei) 1, GL_FALSE, glm::value_ptr(v)); \
		GL_CHECK_ERRORS_MSG(name +" in any of the "+ vshader->file); return true; }


//primitives
UNIFORM_UPLOAD_1(bool, glUniform1i) // bool?
UNIFORM_UPLOAD_1(unsigned char, glUniform1i) // bool?
UNIFORM_UPLOAD_1(unsigned int, glUniform1ui)
UNIFORM_UPLOAD_1(int, glUniform1i)
UNIFORM_UPLOAD_1(float, glUniform1f)

//vectors
UNIFORM_UPLOAD_GLMv(glm::uvec2, glUniform2uiv)
UNIFORM_UPLOAD_GLMv(glm::uvec3, glUniform3uiv)
UNIFORM_UPLOAD_GLMv(glm::uvec4, glUniform4uiv)
UNIFORM_UPLOAD_GLMv(glm::ivec2, glUniform2iv)
UNIFORM_UPLOAD_GLMv(glm::ivec3, glUniform3iv)
UNIFORM_UPLOAD_GLMv(glm::ivec4, glUniform4iv)
UNIFORM_UPLOAD_GLMv(glm::fvec2, glUniform2fv)
UNIFORM_UPLOAD_GLMv(glm::fvec3, glUniform3fv)
UNIFORM_UPLOAD_GLMv(glm::fvec4, glUniform4fv)

//matrices
UNIFORM_UPLOAD_GLMm(glm::fmat2, glUniformMatrix2fv)
UNIFORM_UPLOAD_GLMm(glm::fmat3, glUniformMatrix3fv)
UNIFORM_UPLOAD_GLMm(glm::fmat4, glUniformMatrix4fv)
