#include "smegod_dep.h"

#include "shaders.h"
#include "input_handling.h"
#include "camera.h"
#include "world.h"
#include "geometries.h"
#include "light.h"
#include "particles.h"

const string name = "Window";
shared_ptr<World> world;

double lightning_time = 0;
int lightning_type = 0;
const int LIGHTNING_TYPES = 3;
bool first_light = true;
float lightning_offset = 0;

float lightning_intensity() {
	float lightning = 0;
	if (glfwGetTime() > lightning_time + 2) {
		float nextLightning = ((float)rand() / (float)RAND_MAX) * 10 + 2; //(2-30s) between lightnings
		lightning_type = (int)rand() % LIGHTNING_TYPES;
		lightning_time = glfwGetTime() + nextLightning;
		cout << "Next lightning in " << nextLightning << "s, type: " << lightning_type << endl;
		first_light = true;
	}
	else if (glfwGetTime() > lightning_time) {
		float time =(float) glfwGetTime();
		if (first_light) {
			first_light = false;
			lightning_offset = time;
		}
		time = time - lightning_offset + 1.0f;
		switch (lightning_type)
		{
		case 0: //one peak;
			lightning += glm::pow<float>(glm::sin(time), 1500.f) * 40;
			break;
		case 1: // two peaks
			lightning += glm::pow<float>(glm::sin(time), 1500.f) * 40;
			lightning += glm::pow<float>(glm::sin(time - 0.2f), 500.f) * 20;
			break;
		case 2: // three lightnings
			lightning += glm::pow<float>(glm::sin(time), 2500.f) * 50;
			lightning += glm::pow<float>(glm::sin(time - 0.15f), 300.f) * 30;
			break;
		default:
			break;
		}
	}
	return lightning;
}

double time_start = glfwGetTime();
double time_end = time_start;
double time_delta = 0;
double sum = 0;
int fps = 0;


static bool gl_is_broken = false;
static int error_line = -1;

bool check_errors(const string file, const string function, int line, string msg) {
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
			cout << "GL error caught with error code 0x" << hex << result << dec << ": " << err << ". " << file << ":" << function << "(" << line << ") Message: " << msg << endl;
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
	shared_ptr<ShaderGroup> water_shader = make_shared<ShaderGroup>("water.vert", "water.frag");

	vector<GLchar *> ruVaryings;
	ruVaryings.push_back("gs_pos");
	ruVaryings.push_back("gs_seed");
	ruVaryings.push_back("gs_speed");
	ruVaryings.push_back("gs_random");
	ruVaryings.push_back("gs_type");

	shared_ptr<ShaderGroup> rain_update_shader = make_shared<ShaderGroup>("rain.vert", "rain_update.geom", "rain_update.frag", ruVaryings);
	

	vector<GLchar *> rrVaryings;
	shared_ptr<ShaderGroup> rain_render_shader = make_shared<ShaderGroup>("rain.vert", "rain_render.geom", "rain_render.frag", rrVaryings);



	shared_ptr<RenderTexture> gBloom = make_shared<RenderTexture>(Globals::WIDTH, Globals::HEIGHT, GL_RGBA, GL_RGBA16F, GL_FLOAT);

	// Setup g-buffer
	shared_ptr<RenderTexture> gDiffuse = make_shared<RenderTexture>(Globals::WIDTH, Globals::HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE); // - Diffuse buffer
	shared_ptr<RenderTexture> gNormal = make_shared<RenderTexture>(Globals::WIDTH, Globals::HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);  // - NormalSpecular buffer
	shared_ptr<RenderTexture> gAccLight = make_shared<RenderTexture>(Globals::WIDTH, Globals::HEIGHT, GL_RGBA, GL_RGBA16F, GL_FLOAT);
	shared_ptr<DepthTexture> gDepth = make_shared<DepthTexture>(Globals::WIDTH, Globals::HEIGHT); // - Depth buffer
	shared_ptr<RenderTexture> gRain = make_shared<RenderTexture>(Globals::WIDTH, Globals::HEIGHT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE); //Rain buffer

	vector<shared_ptr<Texture>> gAttachments = { gDiffuse, gNormal, gBloom, gAccLight};
	FrameBuffer gBuffer(gAttachments, gDepth);

	vector<shared_ptr<Texture>> rainAttachments = { gRain };
	FrameBuffer rainBuffer(rainAttachments, gDepth);

	vector<shared_ptr<Texture>> skyboxAttachments = { gDiffuse };
	FrameBuffer skyboxBuffer(skyboxAttachments, gDepth);

	// Setup light buffer
	vector<shared_ptr<Texture>> lAttachments = { gAccLight, gBloom };
	FrameBuffer lBuffer(lAttachments, gDepth);


	// Setup shadow buffer
	shared_ptr<DepthTexture> shadowMap = make_shared<DepthTexture>(Globals::SHADOW_WIDTH, Globals::SHADOW_HEIGHT);
	vector<shared_ptr<Texture>> empty_indeed;
	FrameBuffer sBuffer(empty_indeed, shadowMap);


	//Setup blur step
	shared_ptr<RenderTexture> gPing = make_shared<RenderTexture>(Globals::WIDTH / 2, Globals::HEIGHT / 2, GL_RGBA, GL_RGBA16F, GL_FLOAT);
	shared_ptr<RenderTexture> gPong = make_shared<RenderTexture>(Globals::WIDTH / 2, Globals::HEIGHT / 2, GL_RGBA, GL_RGBA16F, GL_FLOAT);

	vector<shared_ptr<Texture>> pingAttachments = { gPing };
	vector<shared_ptr<Texture>> pongAttachments = { gPong };
	FrameBuffer pingBuffer(pingAttachments);
	FrameBuffer pongBuffer(pongAttachments);

	FrameBuffer* pingpongBuffer[2] = { &pingBuffer, &pongBuffer };
	vector<shared_ptr<Texture>> pingpongTextures = { gPing, gPong };


	//Setup unifroms
	glm::vec2 shadowMapTexelSize = { 1.0f / Globals::SHADOW_WIDTH, 1.0f / Globals::SHADOW_HEIGHT };
	glm::vec2 invRes = { 1.0f / Globals::WIDTH, 1.0f / Globals::HEIGHT};



	shared_ptr<DDSTexture> rainSplashBump = make_shared<DDSTexture>("rainTextures/splashes/SBumpVolume.dds");
	glBindTexture(rainSplashBump->getTarget(), rainSplashBump->getGlId());
	glTexParameteri(rainSplashBump->getTarget(), GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(rainSplashBump->getTarget(), GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(rainSplashBump->getTarget(), GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	glTexParameteri(rainSplashBump->getTarget(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(rainSplashBump->getTarget(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(rainSplashBump->getTarget(), 0);
	shared_ptr<DDSTexture> rainSplashDiffuse = make_shared<DDSTexture>("rainTextures/splashes/SDiffuseVolume.dds");
	glBindTexture(rainSplashDiffuse->getTarget(), rainSplashBump->getGlId());
	glTexParameteri(rainSplashDiffuse->getTarget(), GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(rainSplashDiffuse->getTarget(), GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(rainSplashDiffuse->getTarget(), GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	glTexParameteri(rainSplashDiffuse->getTarget(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(rainSplashDiffuse->getTarget(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(rainSplashDiffuse->getTarget(), 0);

	//rain
	shared_ptr<ArrayTexture> rainTexs = make_shared<ArrayTexture>("rainTextures/cv0_vPositive_%.4d.png", 10, 16, 526, GL_RGBA, GL_UNSIGNED_BYTE, GL_SRGB8_ALPHA8);

	shared_ptr<Texture> tex = Texture::loadFromFile("notex.png");

	Particles rain(rain_update_shader, rain_render_shader);

	// Water
	shared_ptr<Texture> water_bump = Texture::loadFromFile("waves.png");
	Geometry water(ParametricShapes::createSurface(600, 600, 300));
	water.translate(-300, 0, -300);


	auto cubemap = make_shared<Cubemap>("Textures/grimmnight_cubemap/grimmnight_", ".tga");
	auto skybox = make_shared<Skybox>(cubemap);


	vector<shared_ptr<SpotLight>> lights;

	float lightoffset = 21.54f;

	shared_ptr<SpotLight> sl1 = make_shared<SpotLight>(glm::vec3{ 30.f, 30.f, 15.f }, 30.f, 40.f, 0.5f, 0.3f);
	sl1->setTranslate(3.7f, 6.f, -2.9f);
	sl1->rotate(180, 40, 0);

	shared_ptr<SpotLight> sl2 = make_shared<SpotLight>(glm::vec3{ 30.f, 30.f, 15.f }, 30.f, 40.f, 0.5f, 0.3f);
	sl2->translate(-3.7f, 6.f, -2.9f);
	sl2->rotate(180, 40, 0);

	shared_ptr<SpotLight> sl3 = make_shared<SpotLight>(glm::vec3{ 30.f, 30.f, 15.f }, 30.f, 40.f, 0.5f, 0.3f);
	sl3->translate(3.7f + lightoffset, 6.f, -2.9f);
	sl3->rotate(180, 40, 0);

	shared_ptr<SpotLight> sl4 = make_shared<SpotLight>(glm::vec3{ 30.f, 30.f, 15.f }, 30.f, 40.f, 0.5f, 0.3f);
	sl4->translate(-3.7f + lightoffset, 6.f, -2.9f);
	sl4->rotate(180, 40, 0);
	
	Node lhRotator;
	lhRotator.translate(35.56f, 26.3f, -1.31f);
	shared_ptr<SpotLight> lh = make_shared<SpotLight>(glm::vec3{ 5.f, 5.f, 4.f }, 25.f, 30.f, 0.02f, 0.001f,0.35f);

	shared_ptr<SpotLight> lh_top = make_shared<SpotLight>(glm::vec3{7.f, 7.f, 2.0f }, 30.f, 45.f, 2.f, 3.f,0.4f);
	lh_top->translate(35.56f, 26.3f+2.f, -1.31f);
	lh_top->rotate(0, 90, 0);

	lights.push_back(sl1);
	lights.push_back(sl2);

	lights.push_back(sl3);
	lights.push_back(sl4);


	lights.push_back(lh_top);
	lights.push_back(lh);

	vector<shared_ptr<SpotLight>> raineffecting_lights;
	raineffecting_lights.push_back(sl1);
	raineffecting_lights.push_back(sl2);
	raineffecting_lights.push_back(sl3);
	raineffecting_lights.push_back(sl4);
	raineffecting_lights.push_back(lh);

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
	glm::vec3 moon(50, 100, 30);

	//FrameBuffer::printFramebufferLimits();

	world->initiate();
	shared_ptr<Camera> cam = world->active_camera;
	while (!glfwWindowShouldClose(window)) {
		if (Globals::UNIFORM_REFRESH) {
			resolve_shader->use();
			resolve_shader->setUniform("u_inv_res", invRes);

			laccbuff_shader->use();
			laccbuff_shader->setUniform("u_inv_res", invRes);
			laccbuff_shader->setUniform("u_shadow_texelsize", shadowMapTexelSize);

			gbuffer_shader->use();
			gbuffer_shader->setUniform("u_KsDir", 2.0f);
			gbuffer_shader->setUniform("u_splash_offset", glm::vec2(random() * 2, random() * 2));
			gbuffer_shader->setUniform("u_height_scale", 0.055f);

			gbuffer_shader->bindTexture("splash_bump_texture", 3, rainSplashBump);
			gbuffer_shader->bindTexture("splash_texture", 4, rainSplashDiffuse);

			Globals::UNIFORM_REFRESH = false;
		}
		update_delta();
		world->update(time_delta);
			cam->update(time_delta);
			cam->render(cam->world);


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



		//geometry
		gBuffer.activate();
		gbuffer_shader->use();
		static float timeCycle = 0;
		if (Globals::TIME_NOT_FROZEN)
			timeCycle += (float)fps/(16.f*4.f);
		gbuffer_shader->setUniform("u_timeCycle", timeCycle);
		if (timeCycle >= 0.7f)
		{
			timeCycle = 0;
			gbuffer_shader->setUniform("u_splash_offset", glm::vec2(random() * 2, random() * 2));
		}
		gbuffer_shader->setUniform("u_view_projection", cam->view_projection);
		gbuffer_shader->setUniform("u_camera_pos", glm::vec3(cam->combined_world[3]));
		
		world->render(gbuffer_shader);

		//water
		GL_CHECK_ERRORS_MSG("Before water uniforms");
		water_shader->use();
		water_shader->setUniform("time", (float)glfwGetTime());
		water_shader->setUniform("camera_pos", glm::vec3(cam->combined_world[3]));
		water_shader->setUniform("view_projection", cam->view_projection);
		water_shader->bindTexture("bump", 1, water_bump);

		water_shader->bindCubemap("skybox", 0, *cubemap.get());
		

		GL_CHECK_ERRORS_MSG("Before water render");
		water.render(water.world, water_shader, true);
		GL_CHECK_ERRORS_MSG("After water render");

		// skybox
		skyboxBuffer.activate();
		glDepthFunc(GL_LEQUAL);
		skybox_shader->use();
		skybox_shader->setUniform("u_projection", cam->projection);
		skybox_shader->setUniform("u_view", cam->view);
		skybox_shader->bindCubemap("skybox", 0, *cubemap.get());
		skybox->render(ident, skybox_shader, false);
		glDepthFunc(GL_LESS);

		rainBuffer.activate();
		glClear(GL_COLOR_BUFFER_BIT);
		//move rain
		rain_update_shader->use();
		rain_update_shader->setUniform("u_camera_pos", glm::vec3(cam->combined_world[3]));
		rain_update_shader->setUniform("u_total_vel", glm::vec3(0, -8, 0));
		rain_update_shader->setUniform("u_height_range", 35.0f);
		rain_update_shader->setUniform("u_move_particles", Globals::TIME_NOT_FROZEN);
		rain_update_shader->setUniform("u_framerate", (float)1 / (float)time_delta);
		rain.update();

		//render
		rain_render_shader->use();


		for (int i = 0; i < raineffecting_lights.size(); i++) {
			shared_ptr<SpotLight> sl = raineffecting_lights[i];
			string prefix = "lights[" + to_string(i) + "]";
			sl->bindUniform(rain_render_shader, prefix);
		}
		rain_render_shader->setUniform("u_camera_pos", glm::vec3(cam->combined_world[3]));
		rain_render_shader->setUniform("u_total_vel", glm::vec3(0, -0.35, 0));
		rain_render_shader->setUniform("u_sprite_size", 1.f);
		rain_render_shader->setUniform("u_view_projection", cam->view_projection);
		rain_render_shader->bindTexture("rainTextureArray", 0, rainTexs);


		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		rain.renderParticles();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		//swap rain buffers
		rain.swap();

		PERF_END(PassPerf::Pass::GEOMETRY_PASS);
		//
		// PASS 2: Generate shadowmaps and accumulate lights' contribution
		//
		PERF_START(PassPerf::Pass::FULL_LIGHT_PASS);

		laccbuff_shader->use();
		
		laccbuff_shader->setUniform("u_view_projection", cam->view_projection);
		laccbuff_shader->setUniform("u_view_projection_inverse", cam->view_projection_inverse);
		laccbuff_shader->setUniform("u_camera_pos", glm::vec3(cam->combined_world[3]));

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


			//unable to bind 0 to u_specular_power
			glm::mat4 world_to_light = sl->getLightSpaceMatrix();
			shadow_shader->setUniform("u_world_to_light", world_to_light);

			world->render(shadow_shader, false);

			// 2.2 blend light
			laccbuff_shader->use();
			laccbuff_shader->bindTexture("depthBuffer", 0, gDepth);
			laccbuff_shader->bindTexture("normalAndSpecularBuffer", 1, gNormal);
			laccbuff_shader->bindTexture("shadowMap", 2, shadowMap);
			laccbuff_shader->bindTexture("diffuseBuffer", 3, gDiffuse);
			laccbuff_shader->setUniform("u_world_to_light", world_to_light);
			laccbuff_shader->setUniform("u_specular_power", 100.f);

			lBuffer.activate();
			glViewport(0, 0, Globals::WIDTH, Globals::HEIGHT);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glEnable(GL_BLEND);
			glDepthFunc(GL_GREATER);
			glDepthMask(GL_FALSE);

			glBlendEquationSeparate(GL_FUNC_ADD, GL_MIN);
			glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);

			sl->render(sl->world, laccbuff_shader, false);

			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LESS);
			glDisable(GL_BLEND);
		}
		PERF_END(PassPerf::Pass::FULL_LIGHT_PASS);


		glCullFace(GL_BACK);
		glDepthFunc(GL_ALWAYS);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		if (Globals::WIREFRAME) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
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
		
		float lightning = lightning_intensity();

		glDepthMask(GL_FALSE);
		resolve_shader->use();
		resolve_shader->bindTexture("diffuse_buffer", 0, gDiffuse);
		resolve_shader->bindTexture("light_buffer", 1, gAccLight);
		resolve_shader->bindTexture("bloom_buffer", 2, horizontal ? gPing : gPong);
		resolve_shader->bindTexture("rain_buffer", 3, gRain);
		resolve_shader->setUniform("u_lightning", lightning);
		output.render();

	

		PERF_END(PassPerf::Pass::RESOLVE_PASS);

		glDepthMask(GL_TRUE);
		GL_CHECK_ERRORS();

		PERF_START(PassPerf::Pass::QUAD_PASS);
		if (Globals::DRAW_BUFFERS) {
			//Draw debug window
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
		
			buff_shader->setUniform("near", 0.1f);
			buff_shader->setUniform("far", 1.f);
			buff_shader->setUniform("mask", glm::vec3(0, 1.f, 0));
			buff_shader->bindTexture("buff", 0, gDepth);
			quad_depth.render();

			buff_shader->setUniform("mask", glm::vec3(1.f, 0, 0));
			buff_shader->bindTexture("buff", 0, gAccLight);
			quad_acclight.render();

			buff_shader->setUniform("near", 0.1f);
			buff_shader->setUniform("far", 1.f);
			buff_shader->setUniform("mask", glm::vec3(0, 1.f, 0));
			buff_shader->bindTexture("buff", 0, shadowMap);

			quad_shadowmap.render();
		
			buff_shader->setUniform("mask", glm::vec3(1.f, 0, 0));
			buff_shader->bindTexture("buff", 0, gBloom);
			quad_bloom.render();

			buff_shader->setUniform("mask", glm::vec3(1.f, 0, 0));
			buff_shader->bindTexture("buff", 1, gRain);
			quad_ping.render();

		}
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
		lh->world = glm::rotate(lhRotator.world,(float) glfwGetTime()*0.4f, glm::vec3(lhRotator.world[1]));
		lh_top->world = lh->world;
		lh_top->translate(0.9f, .5f, 0);
		lh_top->rotate(-90, 70, 0);
		

		lh->translate(1.7f, 0, 0);
		lh->rotate(-90, 25, 0);
		PERF_PRINT();


#if DEBUG_LEVEL >= 1
		if (!Globals::UNIFORM_REFRESH)
			ShaderGroup::checkUniforms();
#endif
	}
}

int main() {

	if (!glfwInit()) {
		cerr << "Failed to initialize GLFW" << endl;
		exit(EXIT_FAILURE);
	}
	// get version info
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);


	GLFWwindow *window = glfwCreateWindow(Globals::WIDTH, Globals::HEIGHT, name.c_str(), Globals::FULLSCREEN ? glfwGetPrimaryMonitor() : nullptr, nullptr);

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
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	main_loop(window);

	glfwTerminate();

	exit(EXIT_SUCCESS);
}

