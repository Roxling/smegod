#include <memory>
#include <iostream>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

using namespace std;

const int width = 800;
const int height = 600;
const string name = "Window";

static void error_callback(int error, const char* description)
{
	cerr << description << endl;
}

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

		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClearColor(1.f, .7f, .7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef((float)glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
		glBegin(GL_TRIANGLES);
		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(-0.6f, -0.4f, 0.f);
		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(0.6f, -0.4f, 0.f);
		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(0.f, 0.6f, 0.f);
		glEnd();

		/* END RENDER WORLD */
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
}

int main() {
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	GLFWwindow *window(glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr));

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glewExperimental = true;

	if (glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	
	glfwSetKeyCallback(window, key_callback);

	main_loop(window);

	glfwTerminate();

	exit(EXIT_SUCCESS);
}

