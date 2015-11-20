#include "shaders.h"
#include <fstream>
#include <streambuf>

using namespace std;

Shader::Shader(GLenum mtype, string mfile) : type(mtype), file(mfile) {}

Shader::~Shader()
{
	glDeleteShader(shader);
}

Shader::COMPILE_STATUS Shader::compile() {

	ifstream stream(FOLDER + file);
	if (!stream.good()) {
		cout << "File " << FOLDER + file << " does not exist. Can not load (or compile) shader." << endl;
		return Shader::COMPILE_STATUS::FAILED;
	}
	string ncode = string((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());
	stream.close();
	if (code.compare(ncode) == 0) {
		return Shader::COMPILE_STATUS::UNCHANGED;
	}
	else {
		cout << "Compiling " << file << "." << endl;
		glDeleteShader(shader);
	}
	shader = glCreateShader(type);
	const GLchar *glCode = ncode.c_str();
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
	else {
		code = ncode;
	}
	compiled = success ? true : false;
	return compiled ? Shader::COMPILE_STATUS::SUCCESS : Shader::COMPILE_STATUS::FAILED;
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

void Shader::detach()
{
	glDetachShader(program, shader);
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
	shader_program = glCreateProgram();
	all_groups->push_back(this);
	vshader = make_unique<VertexShader>(vs);
	fshader = make_unique<FragmentShader>(fs);
	compile();
}
void ShaderGroup::compile()
{
	auto vs_status = vshader->compile();
	auto fs_status = fshader->compile();

	if (vs_status == Shader::COMPILE_STATUS::UNCHANGED && fs_status == Shader::COMPILE_STATUS::UNCHANGED) {
		cout << "No changes in " << vshader->file << " or " << fshader->file << "." << endl;
		return;
	}

	if (vs_status != Shader::COMPILE_STATUS::FAILED && fs_status != Shader::COMPILE_STATUS::FAILED) {
		
		vshader->attachTo(shader_program);
		fshader->attachTo(shader_program);
		if (link()) {
			vshader->detach();
			fshader->detach();
		}
		else {
			//glDeleteProgram(shader_program);
		}
		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			cerr << "GLerror: 0x" << hex << error << dec << endl;
		}
	}
	else{
		if (vs_status == Shader::COMPILE_STATUS::FAILED)
			cout << vshader->file << " failed to compile. Fix it and try again." << endl;
		if(fs_status == Shader::COMPILE_STATUS::FAILED)
			cout << fshader->file << " failed to compile. Fix it and try again." << endl;
	}
	
}
void ShaderGroup::bindMaterial(shared_ptr<Material> material)
{
	GLuint color_location = glGetUniformLocation(shader_program, "incolor");
	glUniform3fv(color_location, 1, glm::value_ptr(material->color));

	GLuint ambient_loc, diffuse_loc, specular_loc, shininess_loc;
	ambient_loc = glGetUniformLocation(shader_program, "kambient");
	glUniform3fv(ambient_loc, 1, glm::value_ptr(material->ambient));
	diffuse_loc = glGetUniformLocation(shader_program, "kdiffuse");
	glUniform3fv(diffuse_loc, 1, glm::value_ptr(material->diffuse));
	specular_loc = glGetUniformLocation(shader_program, "kspecular");
	glUniform3fv(specular_loc, 1, glm::value_ptr(material->specular));
	shininess_loc = glGetUniformLocation(shader_program, "shininess");
	glUniform1fv(shininess_loc, 1, (const float *)&material->shininess);

	for (int i = 0; i < material->textures.size(); ++i) {
		string name = material->textures[i].first;
		GLuint id = material->textures[i].second;
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, id);
		glUniform1i(glGetUniformLocation(shader_program, name.c_str()), i);
	}
}
bool ShaderGroup::link()
{
	glLinkProgram(shader_program);
	GLint success;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		const int logSize = 512;
		GLchar log[logSize];

		glGetProgramInfoLog(shader_program, logSize, NULL, log);
		cout << "Shader program failed to link." << endl << log << endl;
		return false;
	}
	return true;

}

