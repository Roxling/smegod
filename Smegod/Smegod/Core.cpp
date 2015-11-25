#include "smegod_dep.h"

#include "shaders.h"
#include "input_handling.h"
#include "camera.h"
#include "world.h"
#include "geometries.h"
#include "light.h"

const string name = "Window";
shared_ptr<World> world;


double time_start = glfwGetTime();
double time_end = time_start;
double time_delta = 0;
double sum = 0;
int fps = 0;


static bool gl_is_broken = false;
static int error_line = -1;
bool check_errors(const string file, const string function, int line) {
	GLenum result;
	result = glGetError();

	if (result) {
		if (error_line != line) {
			gl_is_broken = true;
			error_line = line;
			string err;
			switch (result) {
			case GL_INVALID_ENUM: err = "Invalid enum"; break;
			case GL_INVALID_VALUE: err = "Invalid value"; break;
			case GL_INVALID_OPERATION: err = "Invalid operation"; break;
			case GL_STACK_OVERFLOW: err = "Stack overflow"; break;
			case GL_STACK_UNDERFLOW: err = "Stack undeflow"; break;
			case GL_OUT_OF_MEMORY: err = "Out of memory"; break;
			default: err = ""; break;
			}
			cout << "GL error caught with error code 0x" << hex << result << dec << ": " << err << ". " << file << ":" << function << "(" << line << ")" << endl;
		}
		return false;
	}
	else {
		error_line = -1;
	}
	return true;
}

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

	shared_ptr<ShaderGroup> buff_shader = make_shared<ShaderGroup>("buffrender.vert", "buffrender.frag");
	shared_ptr<ShaderGroup> gbuffer_shader = make_shared<ShaderGroup>("gbuffer.vert", "gbuffer.frag");
	shared_ptr<ShaderGroup> shadow_shader = make_shared<ShaderGroup>("shadowmap.vert", "shadowmap.frag");
	shared_ptr<ShaderGroup> laccbuff_shader = make_shared<ShaderGroup>("laccbuffer.vert", "laccbuffer.frag");
	shared_ptr<ShaderGroup> resolve_shader = make_shared<ShaderGroup>("resolve.vert", "resolve.frag");


	// Setup g-buffer
	RenderTexture gDiffuse(Globals::WIDTH, Globals::HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE); // - Diffuse buffer
	RenderTexture gNormal(Globals::WIDTH, Globals::HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);  // - NormalSpecular buffer
	DepthTexture gDepth(Globals::WIDTH, Globals::HEIGHT); // - Depth buffer

	vector<Texture *> gAttachments = { &gDiffuse, &gNormal };
	FrameBuffer gBuffer(&gAttachments, &gDepth);


	// Setup light buffer
	RenderTexture gAccLight(Globals::WIDTH, Globals::HEIGHT, GL_RGBA, GL_RGBA16F, GL_FLOAT);

	vector<Texture *> lAttachments = { &gAccLight };
	FrameBuffer lBuffer(&lAttachments, &gDepth);


	// Setup shadow buffer
	DepthTexture shadowMap(Globals::SHADOW_WIDTH, Globals::SHADOW_HEIGHT);
	FrameBuffer sBuffer(nullptr, &shadowMap);


	vector<shared_ptr<SpotLight>> lights;

	Node lg;


	shared_ptr<SpotLight> sl1 = make_shared<SpotLight>(laccbuff_shader);
	sl1->translate(0, .3f, 0);
	sl1->LightColor = { 1.f, 0.5f, 0.5f };

	shared_ptr<SpotLight> sl2 = make_shared<SpotLight>(laccbuff_shader);
	sl2->translate(0, .3f, 0);
	sl2->world = glm::rotate(sl2->world, glm::pi<float>(), glm::vec3(sl2->world[1]));
	sl2->LightColor = { .5f, 1.f, 0.5f };

	shared_ptr<SpotLight> sl3 = make_shared<SpotLight>(laccbuff_shader);
	sl3->translate(0, .3f, 0);
	sl3->world = glm::rotate(sl3->world, glm::half_pi<float>(), glm::vec3(sl3->world[1]));
	sl3->LightColor = { .5f, 0.5f, 1.f };

	shared_ptr<SpotLight> sl4 = make_shared<SpotLight>(laccbuff_shader);
	sl4->translate(0, .3f, 0);
	sl4->world = glm::rotate(sl4->world, -glm::half_pi<float>(), glm::vec3(sl4->world[1]));
	sl4->LightColor = { 1.f, 0.5f, 1.f };

	lg.attach(sl1);
	lg.attach(sl2);

	lights.push_back(sl1);
	lights.push_back(sl2);
	lights.push_back(sl3);
	lights.push_back(sl4);

	shared_ptr<Geometry> output = make_shared<Geometry>(ParametricShapes::createNDCQuad(-1, -1, 2, 2));


	//Init debug quads
	shared_ptr<Geometry> textures = make_shared<Geometry>(ParametricShapes::createNDCQuad(-1, -1, 0.4f, 0.4f));
	shared_ptr<Geometry> normals = make_shared<Geometry>(ParametricShapes::createNDCQuad(-.6f, -1, 0.4f, 0.4f));
	shared_ptr<Geometry> speculars = make_shared<Geometry>(ParametricShapes::createNDCQuad(-.2f, -1, 0.4f, 0.4f));
	shared_ptr<Geometry> depth = make_shared<Geometry>(ParametricShapes::createNDCQuad(.2f, -1, 0.4f, 0.4f));
	shared_ptr<Geometry> accumulatedlight = make_shared<Geometry>(ParametricShapes::createNDCQuad(.6f, -1, 0.4f, 0.4f));
	shared_ptr<Geometry> shadowmap = make_shared<Geometry>(ParametricShapes::createNDCQuad(-1, 0.6f, 0.4f, 0.4f));
	textures->bindTexture("buff", gDiffuse.getGlId());
	normals->bindTexture("buff", gNormal.getGlId());
	speculars->bindTexture("buff", gNormal.getGlId());
	depth->bindTexture("buff", gDepth.getGlId());
	accumulatedlight->bindTexture("buff", gAccLight.getGlId());
	shadowmap->bindTexture("buff", shadowMap.getGlId());

	glm::mat4 ident;

	world->initiate();
	world->active_camera->addShaderGroup(gbuffer_shader);
	world->active_camera->addShaderGroup(laccbuff_shader);
	while (!glfwWindowShouldClose(window)) {
		update_delta();
		world->update(time_delta);
		world->active_camera->update(time_delta);
		world->active_camera->render(world->active_camera->world);

		// 1. Geometry Pass: render scene's geometry/color data into gbuffer

		glDepthFunc(GL_LESS);

		gBuffer.activate();
		glViewport(0, 0, Globals::WIDTH, Globals::HEIGHT);
		glClearDepthf(1.0f);
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		gbuffer_shader->use();
		/* START RENDER WORLD */
		
		world->render(gbuffer_shader);
		/* END RENDER WORLD */
		


		//
		// PASS 2: Generate shadowmaps and accumulate lights' contribution
		//
		lBuffer.activate();
		glViewport(0, 0, Globals::WIDTH, Globals::HEIGHT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		for (int i = 0; i < lights.size(); i++) {
			shared_ptr<SpotLight> sl = lights[i];

			sBuffer.activate();
			glCullFace(GL_FRONT);
			glClearDepthf(1.0f);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

			// 2.1 shadowmap
			shadow_shader->use();
			glViewport(0, 0, Globals::SHADOW_WIDTH, Globals::SHADOW_HEIGHT);


			glm::mat4 model_to_clip_matrix = sl->getLightSpaceMatrix();
			shadow_shader->setUniform("model_to_clip_matrix", model_to_clip_matrix);

			world->render(shadow_shader);


			laccbuff_shader->use();
			laccbuff_shader->bindTexture("depthBuffer", 0, gDepth);
			laccbuff_shader->bindTexture("normalAndSpecularBuffer", 1, gNormal);
			laccbuff_shader->bindTexture("shadowMap", 2, shadowMap);

			laccbuff_shader->setUniform("worldToLight", model_to_clip_matrix);

			// 2.2 blend light
			lBuffer.activate();
			glViewport(0, 0, Globals::WIDTH, Globals::HEIGHT);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			//glClear(GL_COLOR_BUFFER_BIT);
			glEnable(GL_BLEND);
			glDepthFunc(GL_GREATER);
			glDepthMask(GL_FALSE);

			glBlendEquationSeparate(GL_FUNC_ADD, GL_MIN);
			glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);



			sl->render(lg.world * sl->world, laccbuff_shader);


			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LESS);
			glDisable(GL_BLEND);
		}



		glCullFace(GL_BACK);
		glDepthFunc(GL_ALWAYS);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// PASS 3 -- Resolve
		glClearColor(1.f, .1f, .7f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		
		glDepthMask(GL_FALSE);

		resolve_shader->use();
		resolve_shader->bindTexture("diffuse_buffer", 0, gDiffuse);
		resolve_shader->bindTexture("light_buffer", 1, gAccLight);

		output->render(ident, resolve_shader);

		glDepthMask(GL_TRUE);


		//Draw debug window
		buff_shader->use();


		buff_shader->setUniform("mask", glm::vec3(1.f, 0, 0));
		textures->render(ident, buff_shader);
		
		buff_shader->setUniform("mask", glm::vec3(1.f, 0, 0));
		normals->render(ident, buff_shader);
		
		buff_shader->setUniform("mask", glm::vec3(0, 0, 0));
		speculars->render(ident, buff_shader);
		
		buff_shader->setUniform("mask", glm::vec3(0, 1.f, 0));
		depth->render(ident, buff_shader);

		buff_shader->setUniform("mask", glm::vec3(1.f, 0, 0));
		accumulatedlight->render(ident, buff_shader);

		buff_shader->setUniform("mask", glm::vec3(0, 1.f, 0));
		shadowmap->render(ident, buff_shader);

		glfwSwapBuffers(window);
		glfwPollEvents();

		//prints GLerrors if any.. Not good for performance and should only be used for debug. Will spam if error occurs every frame.
		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			//cerr << "GLerror: 0x" << hex << error << dec << endl;
		}

		//Update world!

		//lg.world = glm::translate(ident, glm::vec3(10*glm::sin(glfwGetTime()*0.1), 2 * glm::sin(glfwGetTime()*0.3)+ 2, 0));
		lg.world = glm::rotate(ident,(float) glfwGetTime(), glm::vec3(lg.world[1]));
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
	world = make_shared<SponzaWorld>();
	//world = make_shared<ExampleWorld>();
	//world = make_shared<WaterWorld>();

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

	glViewport(0, 0, Globals::WIDTH, Globals::HEIGHT);

	CommandHandler::print_help();
	main_loop(window);

	glfwTerminate();

	exit(EXIT_SUCCESS);
}

