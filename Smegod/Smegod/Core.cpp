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
	PERF_INIT();

	shared_ptr<ShaderGroup> buff_shader = make_shared<ShaderGroup>("buffrender.vert", "buffrender.frag");
	shared_ptr<ShaderGroup> gbuffer_shader = make_shared<ShaderGroup>("gbuffer.vert", "gbuffer.frag");
	shared_ptr<ShaderGroup> shadow_shader = make_shared<ShaderGroup>("shadowmap.vert", "shadowmap.frag");
	shared_ptr<ShaderGroup> laccbuff_shader = make_shared<ShaderGroup>("laccbuffer.vert", "laccbuffer.frag");
	shared_ptr<ShaderGroup> resolve_shader = make_shared<ShaderGroup>("resolve.vert", "resolve.frag");
	shared_ptr<ShaderGroup> bloom_shader = make_shared<ShaderGroup>("bloom.vert", "bloom.frag");
	shared_ptr<ShaderGroup> skybox_shader = make_shared<ShaderGroup>("cubemap.vert", "cubemap.frag");

	RenderTexture gBloom(Globals::WIDTH, Globals::HEIGHT, GL_RGBA, GL_RGBA16F, GL_FLOAT);

	// Setup g-buffer
	RenderTexture gDiffuse(Globals::WIDTH, Globals::HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE); // - Diffuse buffer
	RenderTexture gNormal(Globals::WIDTH, Globals::HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);  // - NormalSpecular buffer
	DepthTexture gDepth(Globals::WIDTH, Globals::HEIGHT); // - Depth buffer

	vector<Texture *> gAttachments = { &gDiffuse, &gNormal, &gBloom };
	FrameBuffer gBuffer(&gAttachments, &gDepth);


	// Setup light buffer
	RenderTexture gAccLight(Globals::WIDTH, Globals::HEIGHT, GL_RGBA, GL_RGBA16F, GL_FLOAT);

	vector<Texture *> lAttachments = { &gAccLight, &gBloom };
	FrameBuffer lBuffer(&lAttachments, &gDepth);


	// Setup shadow buffer
	DepthTexture shadowMap(Globals::SHADOW_WIDTH, Globals::SHADOW_HEIGHT);
	FrameBuffer sBuffer(nullptr, &shadowMap);


	//Setup blur step
	RenderTexture gPing(Globals::WIDTH / 2, Globals::HEIGHT / 2, GL_RGBA, GL_RGBA16F, GL_FLOAT);
	RenderTexture gPong(Globals::WIDTH / 2, Globals::HEIGHT / 2, GL_RGBA, GL_RGBA16F, GL_FLOAT);

	vector<Texture *> pingAttachments = { &gPing };
	vector<Texture *> pongAttachments = { &gPong };
	FrameBuffer pingBuffer(&pingAttachments);
	FrameBuffer pongBuffer(&pongAttachments);

	FrameBuffer* pingpongBuffer[2] = { &pingBuffer, &pongBuffer };
	Texture* pingpongTextures[2] = { &gPing, &gPong };


	//Setup unifroms
	glm::vec2 shadowMapTexelSize = { 1.0f / Globals::SHADOW_WIDTH, 1.0f / Globals::SHADOW_HEIGHT };
	glm::vec2 invRes = { 1.0f / Globals::WIDTH, 1.0f / Globals::HEIGHT};





	auto cubemap = make_shared<Cubemap>("Textures/grimmnight_cubemap/grimmnight_", ".tga");
	auto skybox = make_shared<Skybox>(cubemap);


	vector<shared_ptr<SpotLight>> lights;

	Node lg;

	float lightoffset = 21.54f;

	shared_ptr<SpotLight> sl1 = make_shared<SpotLight>(laccbuff_shader);
	sl1->translate(3.7f, 6.f, -3.3f);
	sl1->rotate(180, 55, 0);
	sl1->scale(20);
	sl1->LightColor = { 1.f, 1.f, 0.6f };

	shared_ptr<SpotLight> sl2 = make_shared<SpotLight>(laccbuff_shader);
	sl2->translate(-3.7f, 6.f, -3.3f);
	sl2->rotate(180, 55, 0);
	sl2->scale(20);
	sl2->LightColor = { 1.f, 1.f, 0.6f };

	shared_ptr<SpotLight> sl3 = make_shared<SpotLight>(laccbuff_shader);
	sl3->translate(3.7f + lightoffset, 6.f, -3.3f);
	sl3->rotate(180, 55, 0);
	sl3->scale(20);
	sl3->LightColor = { 1.f, 1.f, 0.6f };

	shared_ptr<SpotLight> sl4 = make_shared<SpotLight>(laccbuff_shader);
	sl4->translate(-3.7f + lightoffset, 6.f, -3.3f);
	sl4->rotate(180, 55, 0);
	sl4->scale(20);
	sl4->LightColor = { 1.f, 1.f, 0.6f };

	/*shared_ptr<SpotLight> sl5 = make_shared<SpotLight>(laccbuff_shader);
	sl5->translate(3.7f - lightoffset, 6.f, -3.3f);
	sl5->rotate(180, 55, 0);
	sl5->scale(20);
	sl5->LightColor = { 1.f, 1.f, 0.6f };

	shared_ptr<SpotLight> sl6 = make_shared<SpotLight>(laccbuff_shader);
	sl6->translate(-3.7f - lightoffset, 6.f, -3.3f);
	sl6->rotate(180, 55, 0);
	sl6->scale(20);
	sl6->LightColor = { 1.f, 1.f, 0.6f };
	*/
	
	shared_ptr<SpotLight> lh = make_shared<SpotLight>(laccbuff_shader);
	lh->translate(20,30,0);
	lh->rotate(-90, 45, 0);
	lh->scale(20);
	lh->LightColor = { 1.f, 0, 0.0f };

	lights.push_back(sl1);
	lights.push_back(sl2);

	lights.push_back(sl3);
	lights.push_back(sl4);

	//lights.push_back(sl5);
	//lights.push_back(sl6);

	//lights.push_back(lh);

	Quad output;

	//Init debug quads
	Quad quad_textures(-1.f, -1.f, -.6f, -.6f);
	Quad quad_normals(-.6f, -1.f, -.2f, -.6f);
	Quad quad_speculars(-.2f, -1.f, .2f, -.6f);
	Quad quad_depth(.2f, -1.f, .6f, -.6f);
	Quad quad_acclight(.6f, -1.f, 1.f, -.6f);

	Quad quad_shadowmap(-1.f, .6f, -.6f, 1.f);
	Quad quad_bloom(-.6f, .6f, -.2f, 1.f);
	Quad quad_ping(-.2f, .6f, .2f, 1.f);

	glm::mat4 ident;

	world->initiate();
	shared_ptr<Camera> cam = world->active_camera;
	while (!glfwWindowShouldClose(window)) {
		if (Globals::UNIFORM_REFRESH) {
			resolve_shader->use();
			resolve_shader->setUniform("invRes", invRes);
			laccbuff_shader->use();
			laccbuff_shader->setUniform("invRes", invRes);
			laccbuff_shader->setUniform("shadow_texelsize", shadowMapTexelSize);
			Globals::UNIFORM_REFRESH = false;
		}
		update_delta();
		world->update(time_delta);
		cam->update(time_delta);
		cam->render(cam->world);

		//clear light buffer
		lBuffer.activate();
		glViewport(0, 0, Globals::WIDTH, Globals::HEIGHT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		// 1. Geometry Pass: render scene's geometry/color data into gbuffer
		PERF_START(PassPerf::Pass::GEOMETRY_PASS);

		if (Globals::WIREFRAME) {
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		}

		glDepthFunc(GL_LESS);

		gBuffer.activate();
		glViewport(0, 0, Globals::WIDTH, Globals::HEIGHT);
		glClearDepthf(1.0f);
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		skybox_shader->use();
		skybox_shader->setUniform("projection", cam->projection);
		skybox_shader->setUniform("view", cam->view);
		skybox->render(ident, skybox_shader);

		gbuffer_shader->use();
		gbuffer_shader->setUniform("view_projection", cam->view_projection);
		
		world->render(gbuffer_shader);
		

		PERF_END(PassPerf::Pass::GEOMETRY_PASS);
		//
		// PASS 2: Generate shadowmaps and accumulate lights' contribution
		//
		PERF_START(PassPerf::Pass::FULL_LIGHT_PASS);

		laccbuff_shader->use();
		laccbuff_shader->setUniform("view_projection", cam->view_projection);
		laccbuff_shader->setUniform("view_projection_inverse", cam->view_projecion_inverse);
		laccbuff_shader->setUniform("camera_pos", glm::vec3(cam->combined_world[3]));


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



			// 2.2 blend light
			laccbuff_shader->use();
			laccbuff_shader->bindTexture("depthBuffer", 0, gDepth);
			laccbuff_shader->bindTexture("normalAndSpecularBuffer", 1, gNormal);
			laccbuff_shader->bindTexture("shadowMap", 2, shadowMap);
			laccbuff_shader->setUniform("worldToLight", model_to_clip_matrix);

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
		PERF_END(PassPerf::Pass::FULL_LIGHT_PASS);


		glCullFace(GL_BACK);
		glDepthFunc(GL_ALWAYS);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// PASS 2.3 Blur bloom buffer
		PERF_START(PassPerf::Pass::BLOOM_PASS);
		glViewport(0, 0, Globals::WIDTH / 2, Globals::HEIGHT / 2);
		GLboolean horizontal = true, first_iteration = true;
		GLuint amount = 4;
		bloom_shader->use();
		for (GLuint i = 0; i < amount; i++)
		{
			pingpongBuffer[horizontal]->activate();
			bloom_shader->setUniform("horizontal", horizontal);
			if (first_iteration) {
				bloom_shader->bindTexture("image", 0, gBloom);
				first_iteration = false;
			}
			else {
				bloom_shader->bindTexture("image", 0, horizontal? gPing : gPong);
			}
			output.render();
			horizontal = !horizontal;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		PERF_END(PassPerf::Pass::BLOOM_PASS);

		// PASS 3 -- Resolve
		PERF_START(PassPerf::Pass::RESOLVE_PASS);
		glViewport(0, 0, Globals::WIDTH, Globals::HEIGHT);
		glClearColor(1.f, .1f, .7f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		
		glDepthMask(GL_FALSE);

		if (Globals::WIREFRAME) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}


		resolve_shader->use();
		resolve_shader->bindTexture("diffuse_buffer", 0, gDiffuse);
		resolve_shader->bindTexture("light_buffer", 1, gAccLight);
		resolve_shader->bindTexture("bloom_buffer", 2, horizontal ? gPing : gPong);

		output.render();

		PERF_END(PassPerf::Pass::RESOLVE_PASS);

		glDepthMask(GL_TRUE);
		GL_CHECK_ERRORS();


		//Draw debug window
		PERF_START(PassPerf::Pass::QUAD_PASS);
		buff_shader->use();


		buff_shader->setUniform("mask", glm::vec3(1.f, 0, 0));
		buff_shader->bindTexture("buff", 0, gDiffuse);
		quad_textures.render();
		
		buff_shader->setUniform("mask", glm::vec3(1.f, 0, 0));
		buff_shader->bindTexture("buff", 0, gNormal);
		quad_normals.render();
		
		buff_shader->setUniform("mask", glm::vec3(0, 0, 0));
		buff_shader->bindTexture("buff", 0, gNormal);
		quad_speculars.render();
		
		buff_shader->setUniform("mask", glm::vec3(0, 1.f, 0));
		buff_shader->bindTexture("buff", 0, gDepth);
		quad_depth.render();

		buff_shader->setUniform("mask", glm::vec3(1.f, 0, 0));
		buff_shader->bindTexture("buff", 0, gAccLight);
		quad_acclight.render();

		buff_shader->setUniform("mask", glm::vec3(0, 1.f, 0));
		buff_shader->bindTexture("buff", 0, shadowMap);
		quad_shadowmap.render();
		
		buff_shader->setUniform("mask", glm::vec3(1.f, 0, 0));
		buff_shader->bindTexture("buff", 0, gBloom);
		quad_bloom.render();

		buff_shader->setUniform("mask", glm::vec3(1.f, 0, 0));
		buff_shader->bindTexture("buff", 0, gPing);
		quad_ping.render();

		PERF_END(PassPerf::Pass::QUAD_PASS);

		//Swap buffers
		PERF_START(PassPerf::Pass::SWAP_PASS);
		glfwSwapBuffers(window);
		PERF_END(PassPerf::Pass::SWAP_PASS);

		glfwPollEvents();


		//prints GLerrors if any.. Not good for performance and should only be used for debug. Will spam if error occurs every frame.
#if DEBUG_LEVEL >= 1
		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			cerr << "GLerror: 0x" << hex << error << dec << endl;
		}
#endif

		//Update world!

		//lg.world = glm::translate(ident, glm::vec3(10*glm::sin(glfwGetTime()*0.1), 2 * glm::sin(glfwGetTime()*0.3)+ 2, 0));
		//lg.world = glm::rotate(ident,(float) glfwGetTime(), glm::vec3(lg.world[1]));

		PERF_PRINT();
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

