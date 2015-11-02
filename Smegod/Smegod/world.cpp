#include "world.h"
#include "shaders.h"
#include "geometries.h"
#include "textures.h"
#include "cubemap.h"
#include "light.h"
#include "parametric_shapes.h"
#include "game.h"

World::World()
{
	head = make_shared<Node>();
}

void World::render()
{
	head->render(world_pos);
}

vector<shared_ptr<Node>> cube_groups;
shared_ptr<Node> light_rotator;
int num_objects = 3;
float offset = 50.f;
float dist = 3.f;

void ExampleWorld::initiate()
{
	active_camera = make_shared<Camera>(45.f, Globals::WIDTH, Globals::HEIGHT, 0.1f, 100.f);
	head->attach(active_camera);
	
	shared_ptr<ShaderGroup> n_shader = make_shared<ShaderGroup>("phong.vs", "phong.fs");
	shared_ptr<ShaderGroup> l_shader = make_shared<ShaderGroup>("light.vs", "light.fs");
	shared_ptr<ShaderGroup> norm_shader = make_shared<ShaderGroup>("normal.vs", "normal.fs");
	active_camera->addShaderGroup(norm_shader);
	active_camera->addShaderGroup(n_shader);
	active_camera->addShaderGroup(l_shader);

	light_rotator = make_shared<Node>();
	light_rotator->translate(num_objects*dist, 0, num_objects*dist);
	shared_ptr<Light> light = make_shared<Light>(l_shader);
	light->addShaderGroup(n_shader);
	
	shared_ptr<Texture> tex = make_shared<Texture>("stone43_diffuse.jpg");
	shared_ptr<Texture> bump = make_shared<Texture>("stone43_bump.jpg");

	for (int i = 0; i < num_objects; i++) {
		for (int j = 0; j < num_objects; j++) {
			shared_ptr<Node> g = make_shared<Node>();
			for (int k = 0; k < num_objects; k++) {

				shared_ptr<Geometry> c = make_shared<Geometry>(n_shader, ParametricShapes::createTorus(1,0.5f, 100, 100));
				c->translate(dist*i, dist*k, dist * j);
				c->texture = tex;
				c->bumpmap = bump;
				g->attach(c);
			}
			cube_groups.push_back(g);
			head->attach(g);
		}
	}

	auto cube = make_shared<Geometry>(norm_shader, ParametricShapes::createCube(2.f,10));
	cube->translate(0.f, 0.f, -2.f);
	cube->texture = tex;
	cube->bumpmap = bump;
	head->attach(cube);

	auto surf = make_shared<Geometry>(n_shader, ParametricShapes::createSurface(3,3, 100));
	surf->translate(-5.f, 0.f, -5.f);
	surf->texture = tex;
	head->attach(surf);

	auto nanosuit = make_shared<Geometry>(n_shader, Model("nanosuit/nanosuit.obj"));
	nanosuit->translate(0.f, 0.f, -20.f);
	nanosuit->texture = tex;
	nanosuit->bumpmap = bump;
	head->attach(nanosuit);

	light->translate(-num_objects*2*dist, 0, -num_objects*2*dist);
	light_rotator->attach(light);
	head->attach(light_rotator);

	shared_ptr<Frame> f = make_shared<Frame>(l_shader);
	head->attach(f);
	
}
void ExampleWorld::update(double delta)
{

	active_camera->update(delta);
	light_rotator->world = glm::rotate(world_pos,(float) glfwGetTime()/10, { 1.f,0,-1.f });
	for (int i = 0; i < num_objects; i++) {
		for (int j = 0; j < num_objects; j++) {
			cube_groups[i*num_objects + j]->world = glm::translate(world_pos, { 0,glm::sin(glfwGetTime() + i*glm::radians(offset) + j*glm::radians(offset)) , 0 });
		}
	}
}

shared_ptr<ShaderGroup> water_shader;
shared_ptr<Geometry> surf;
shared_ptr<Plane> plane;

void WaterWorld::initiate()
{
	float size = 30000;

	auto simple_shader = make_shared<ShaderGroup>("phong.vs", "phong.fs");
	plane = make_shared<Plane>(simple_shader);
	plane->world = glm::rotate(plane->world, glm::radians<float>(180), { 0,1,0 });
	plane->translate(0, 100,0);

	head->attach(plane);

	//active_camera = make_shared<Camera>();
	active_camera = make_shared<FlightCamera>(plane);
	active_camera->addShaderGroup(simple_shader);
	
	plane->attach(active_camera);
	//head->attach(active_camera);

	water_shader = make_shared<ShaderGroup>("water.vs", "water.fs");
	active_camera->addShaderGroup(water_shader);


	
	auto cubemap = make_shared<Cubemap>("Textures/opensea_cubemap/opensea_", ".png");
	auto skybox = make_shared<Skybox>(cubemap);
	active_camera->addShaderGroup(skybox->shader_group);
	head->attach(skybox);


	shared_ptr<Texture> bump = make_shared<Texture>("waves.png");

	surf = make_shared<Geometry>(water_shader, ParametricShapes::createInfSurface(size,1200,1200));
	surf->translate(0, -15.0, 0);
	surf->bumpmap = bump;
	head->attach(surf);


	
	auto light_shader = make_shared<ShaderGroup>("light.vs", "light.fs");
	active_camera->addShaderGroup(light_shader);
	//Game geometries
	auto light = make_shared<Light>(light_shader);
	light->addShaderGroup(simple_shader);
	light->translate(-100000, 100000, 90000);
	head->attach(light);

	
	
	auto phong_col = make_shared<ShaderGroup>("phong.vs", "phongcol.fs");
	active_camera->addShaderGroup(phong_col);
	light->addShaderGroup(phong_col);
	GenerateGameWorld(head, phong_col);

	


	/*auto cube = make_shared<Geometry>(phong_col, ParametricShapes::createCube(50.f, 10));
	cube->translate(0.f, 0.f, -2.f);
	cube->color = { 1.0, 1.0, 0.0 };
	head->attach(cube);*/

	shared_ptr<Texture> tex = make_shared<Texture>("stone43_diffuse.jpg");
	shared_ptr<Texture> bump2 = make_shared<Texture>("stone43_bump.jpg");

	auto sf = make_shared<Geometry>(simple_shader, ParametricShapes::createSphere(50.f, 100, 100));
	sf->translate(0.f, 0.f, -2.f);
	sf->texture = tex;
	sf->bumpmap = bump2;
	head->attach(sf);

}

void WaterWorld::update(double delta)
{
	plane->propell(delta);
	active_camera->update(delta);


	//plane->world = world_pos;
	//plane->translate(active_camera->position.x+50, active_camera->position.y-10, active_camera->position.z);
	//surf->world = glm::translate(world_pos, active_camera->position);
	
	water_shader->use();
	auto time_loc = glGetUniformLocation(water_shader->getProgram(), "time");
	float time =(float) glfwGetTime();
	glUniform1fv(time_loc, 1, (const float *)& time);
}

