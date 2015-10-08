#include "smegod_dep.h"

#include "shaders.h"
#include "input_handling.h"
#include "camera.h"
#include "world.h"

const string name = "Window";
shared_ptr<World> world;


double time_start = glfwGetTime();
double time_end = time_start;
double time_delta = 0;
double sum = 0;
int fps = 0;

static void update_delta() {
	time_delta = time_end - time_start;
	time_start = time_end;
	time_end = glfwGetTime();

	sum += time_delta;
	if (sum < 1) {
		fps++;
	}
	else {
		if (Globals::DRAW_FPS) {
			cout << fps << endl;
		}
		fps = 0;
		sum = 0;
	}
}

void print_errors() {
	for (GLenum currError = glGetError(); currError != GL_NO_ERROR; currError = glGetError())
	{
		cout << "ERRORS: " << currError << endl;
	}
}

void main_loop(GLFWwindow* window) {

	world->initiate();
	while (!glfwWindowShouldClose(window)) {
		update_delta();


		glClearColor(1.f, .7f, .7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		/* START RENDER WORLD */
		world->update(time_delta);
		world->render();
		/* END RENDER WORLD */


		glfwSwapBuffers(window);
		glfwPollEvents();

		//prints GLerrors if any.. Not good for performance and should only be used for debug. Will spam if error occurs every frame.
		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			cerr << "GLerror: 0x" << hex << error << dec << endl;
		}
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


	GLFWwindow *window = glfwCreateWindow(Globals::WIDTH, Globals::HEIGHT, name.c_str(), nullptr, nullptr);

	if (window == nullptr) {
		cerr << "Failed to create window" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//world = make_shared<ExampleWorld>();
	world = make_shared<WaterWorld>();

	glfwMakeContextCurrent(window);
	InputHandler::active_window = window;
	glfwSetKeyCallback(window, &InputHandler::key_callback);
	glfwSetCursorPosCallback(window, &InputHandler::mouse_callback);
	glfwSetMouseButtonCallback(window, &InputHandler::mouse_button_callback);


	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	while (GLenum currError = glGetError() != GL_NO_ERROR); //poll all errors from glewInit().
	
	glEnable(GL_DEPTH_TEST);


	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);


	/* Texture options */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glViewport(0, 0, Globals::WIDTH, Globals::HEIGHT);

	CommandHandler::print_help();
	main_loop(window);

	glfwTerminate();

	exit(EXIT_SUCCESS);
}

