#include <memory>
#include <iostream>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include "shaders.h"

using namespace std;

const GLuint WIDTH = 800, HEIGHT = 600;

const string name = "Window";

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void main_loop(GLFWwindow* window) {
	double time_start = glfwGetTime();
	double time_end = time_start;
	double time_delta = 0;
	double sum = 0;
	int fps = 0;
	while (!glfwWindowShouldClose(window)) {
		time_delta = time_end - time_start;
		time_start = time_end;
		time_end = glfwGetTime();

		sum += time_delta;
		if (sum < 1) {
			fps++;
		}
		else {
			cout << fps << endl;
			fps = 0;
			sum = 0;
		}
		
		/* START RENDER WORLD */

		glClearColor(1.f, .7f, .7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		

		/* END RENDER WORLD */
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
}

int main() {

	if (!glfwInit()) {
		cerr << "Failed to initialize GLFW" << endl;
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);


	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, name.c_str(), nullptr, nullptr);

	if (window == nullptr) {
		cerr << "Failed to create window" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	
	auto shader_program = glCreateProgram();
	if(1){ /* Scope for clearing shaders after load*/
		VertexShader basic_vertex ("basic_vertex_shader.glsl");
		basic_vertex.compile();
		basic_vertex.attachTo(shader_program);
		
		PixelShader basic_pixel ("basic_pixel_shader.glsl");
		basic_pixel.compile();
		basic_pixel.attachTo(shader_program);

		glLinkProgram(shader_program);

		GLint success;
		glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
		if (!success) {
			const int logSize = 512;
			GLchar log[logSize];

			glGetProgramInfoLog(shader_program, logSize, NULL, log);
			cout << "Shader program failed to link." << endl << log << endl;
		}
		else {
			glUseProgram(shader_program);
		}
	}


	glViewport(0, 0, WIDTH, HEIGHT);

	main_loop(window);

	glfwTerminate();

	exit(EXIT_SUCCESS);
}

