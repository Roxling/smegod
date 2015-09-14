#include <memory>
#include <iostream>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

using namespace std;

const int width = 800;
const int height = 600;
const string name = "Window";

int main() {
	if (!glfwInit()) {
		cout << "Failed to initialize GLFW" << endl;
		return -1;
	}
	GLFWwindow *window(glfwCreateWindow(width, height, name.c_str(), NULL, NULL));

	if (!window) {
		cout << "Failed to create window" << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = true;

	if (glewInit() != GLEW_OK) {
		cout << "Failed to initialize GLEW" << endl;
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	do {

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window));

	glfwTerminate();
	return 0;
}