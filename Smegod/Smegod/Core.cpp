#include "gl_dep.h"
#include <memory>
#include <iostream>
#include "static_data.h"
#include "shaders.h"
#include "input_handling.h"
#include "camera.h"
#include "world.h"



using namespace std;

const string name = "Window";
shared_ptr<World> world;
unique_ptr<InputHandler> input;


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	input->key_callback(window, key, scancode, action, mods);
}

double time_start = glfwGetTime();
double time_end = time_start;
double time_delta = 0;
double sum = 0;
int fps = 0;

static double get_delta() {
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
	return time_delta;
}

void link_shader_program() {
	auto shader_program = world->active_shader_program;
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

void main_loop(GLFWwindow* window) {

	world->initiate();
	link_shader_program();

	while (!glfwWindowShouldClose(window)) {
		double delta = get_delta();
		
		glClearColor(1.f, .7f, .7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		/* START RENDER WORLD */
		world->update();
		world->render();
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

	world = make_shared<World>();
	input = make_unique<InputHandler>(world);

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glViewport(0, 0, WIDTH, HEIGHT);

	main_loop(window);

	glfwTerminate();

	exit(EXIT_SUCCESS);
}

