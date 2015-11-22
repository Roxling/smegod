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

	shared_ptr<ShaderGroup> buff_shader = make_shared<ShaderGroup>("buffrender.vs", "buffrender.fs");
	shared_ptr<ShaderGroup> gbuffer_shader = make_shared<ShaderGroup>("gbuffer.vs", "gbuffer.fs");
	shared_ptr<ShaderGroup> shadow_shader = make_shared<ShaderGroup>("shadowmap.vs", "shadowmap.fs");
	shared_ptr<ShaderGroup> laccbuff_shader = make_shared<ShaderGroup>("laccbuffer.vs", "laccbuffer.fs");
	shared_ptr<ShaderGroup> resolve_shader = make_shared<ShaderGroup>("resolve.vs", "resolve.fs");


	GLuint gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	GLuint gDiffuse, gNormal, gDepth;

	// - Diffuse buffer
	glGenTextures(1, &gDiffuse);
	glBindTexture(GL_TEXTURE_2D, gDiffuse);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Globals::WIDTH, Globals::HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gDiffuse, 0);

	// - NormalSpecular buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Globals::WIDTH, Globals::HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	

	// - Depth buffer
	glGenTextures(1, &gDepth);
	glBindTexture(GL_TEXTURE_2D, gDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Globals::WIDTH, Globals::HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);

	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	GLuint gattachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, gattachments);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	GLuint lBuffer;
	glGenFramebuffers(1, &lBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, lBuffer);
	
	GLuint accLight;
	// - Accumulative light buffer
	glGenTextures(1, &accLight);
	glBindTexture(GL_TEXTURE_2D, accLight);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Globals::WIDTH, Globals::HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accLight, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);


	GLuint sBuffer;
	glGenFramebuffers(1, &sBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, sBuffer);
	// - Depth buffer
	GLuint shadowMap;
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Globals::SHADOW_WIDTH, Globals::SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat c[4] = { 1.f, 1.f,1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, c);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 

	// - Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	vector<shared_ptr<SpotLight>> lights;

	Node lg;


	shared_ptr<SpotLight> sl1 = make_shared<SpotLight>(laccbuff_shader);
	sl1->translate(0, .1f, 0);
	sl1->LightColor = { 1.f, 0.5f, 0.5f };

	shared_ptr<SpotLight> sl2 = make_shared<SpotLight>(laccbuff_shader);
	sl2->translate(0, .1f, 0);
	sl2->world = glm::rotate(sl2->world, glm::pi<float>(), glm::vec3(sl2->world[1]));
	sl2->LightColor = { .5f, 1.f, 0.5f };

	shared_ptr<SpotLight> sl3 = make_shared<SpotLight>(laccbuff_shader);
	sl3->translate(0, .1f, 0);
	sl3->world = glm::rotate(sl3->world, glm::half_pi<float>(), glm::vec3(sl3->world[1]));
	sl3->LightColor = { .5f, 0.5f, 1.f };

	shared_ptr<SpotLight> sl4 = make_shared<SpotLight>(laccbuff_shader);
	sl4->translate(0, .1f, 0);
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
	textures->bindTexture("buff", gDiffuse);
	normals->bindTexture("buff", gNormal);
	speculars->bindTexture("buff", gNormal);
	depth->bindTexture("buff", gDepth);
	accumulatedlight->bindTexture("buff", accLight);
	shadowmap->bindTexture("buff", shadowMap);


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

		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
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
		
		glBindFramebuffer(GL_FRAMEBUFFER, lBuffer);
		glViewport(0, 0, Globals::WIDTH, Globals::HEIGHT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		for (int i = 0; i < lights.size(); i++) {
			shared_ptr<SpotLight> sl = lights[i];

			glBindFramebuffer(GL_FRAMEBUFFER, sBuffer);
			glCullFace(GL_FRONT);
			glClearDepthf(1.0f);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

			// 2.1 shadowmap
			shadow_shader->use();
			glViewport(0, 0, Globals::SHADOW_WIDTH, Globals::SHADOW_HEIGHT);


			glm::mat4 model_to_clip_matrix = sl->getLightSpaceMatrix();

			glUniformMatrix4fv(glGetUniformLocation(shadow_shader->getProgram(), "model_to_clip_matrix"), 1, GL_FALSE, glm::value_ptr(model_to_clip_matrix));

			world->render(shadow_shader);

			//shadowMap
			laccbuff_shader->use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gDepth);
			glUniform1i(glGetUniformLocation(laccbuff_shader->getProgram(), "depthBuffer"), 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gNormal);
			glUniform1i(glGetUniformLocation(laccbuff_shader->getProgram(), "normalAndSpecularBuffer"), 1);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, shadowMap);
			glUniform1i(glGetUniformLocation(laccbuff_shader->getProgram(), "shadowMap"), 2);

			glUniformMatrix4fv(glGetUniformLocation(laccbuff_shader->getProgram(), "worldToLight"), 1, GL_FALSE, glm::value_ptr(model_to_clip_matrix));

			// 2.2 blend light
			glBindFramebuffer(GL_FRAMEBUFFER, lBuffer);
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


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gDiffuse);
		glUniform1i(glGetUniformLocation(resolve_shader->getProgram(), "diffuse_buffer"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, accLight);
		glUniform1i(glGetUniformLocation(resolve_shader->getProgram(), "light_buffer"), 1);

		output->render(ident, resolve_shader);
		glDepthMask(GL_TRUE);


		//Draw debug window
		buff_shader->use();
		GLuint maskpos = glGetUniformLocation(buff_shader->getProgram(), "mask");
		glUniform3fv(maskpos, 1, glm::value_ptr(glm::vec3(1.f, 0, 0)));
		textures->render(ident, buff_shader);
		glUniform3fv(maskpos, 1, glm::value_ptr(glm::vec3(1.f, 0, 0)));
		normals->render(ident, buff_shader);
		glUniform3fv(maskpos, 1, glm::value_ptr(glm::vec3(0, 0, 0)));
		speculars->render(ident, buff_shader);
		glUniform3fv(maskpos, 1, glm::value_ptr(glm::vec3(0, 1, 0)));
		depth->render(ident, buff_shader);
		glUniform3fv(maskpos, 1, glm::value_ptr(glm::vec3(1.f, 0, 0)));
		accumulatedlight->render(ident, buff_shader);
		glUniform3fv(maskpos, 1, glm::value_ptr(glm::vec3(0, 1, 0)));
		shadowmap->render(ident, buff_shader);


		glfwSwapBuffers(window);
		glfwPollEvents();

		//prints GLerrors if any.. Not good for performance and should only be used for debug. Will spam if error occurs every frame.
		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			//cerr << "GLerror: 0x" << hex << error << dec << endl;
		}

		//Update world!

		lg.world = glm::translate(ident, glm::vec3(10*glm::sin(glfwGetTime()*0.1), 2 * glm::sin(glfwGetTime()*0.3)+ 2, 0));
		lg.world = glm::rotate(lg.world,(float) glfwGetTime(), glm::vec3(lg.world[1]));
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

