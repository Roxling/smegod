#include "world.h"
#include "shaders.h"
#include "geometries.h"
#include "textures.h"
#include "light.h"
#include "parametric_shapes.h"

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
int max = 0;
float offset = 50.f;
float dist = 3.f;

void ExampleWorld::initiate()
{
	
	active_camera = make_shared<Camera>(45.f, Globals::WIDTH, Globals::HEIGHT, 0.1f, 100.f);
	head->attach(active_camera);
	
	shared_ptr<ShaderGroup> n_shader = make_shared<ShaderGroup>("phong.vs", "phong.fs");
	shared_ptr<ShaderGroup> l_shader = make_shared<ShaderGroup>("light.vs", "light.fs");
	active_camera->addShaderGroup(n_shader);
	active_camera->addShaderGroup(l_shader);

	light_rotator = make_shared<Node>();
	light_rotator->translate(max*dist, 0, max*dist);
	shared_ptr<Light> light = make_shared<Light>(l_shader);
	light->addShaderGroup(n_shader);
	
	shared_ptr<Texture> tex = make_shared<Texture>("wood.jpg");

	for (int i = 0; i < max; i++) {
		for (int j = 0; j < max; j++) {
			shared_ptr<Node> g = make_shared<Node>();
			for (int k = 0; k < max; k++) {

				shared_ptr<Geometry> c = make_shared<Geometry>(n_shader, ParametricShapes::createTorus(1,0.5f, 100, 100));
				c->translate(dist*i, dist*k, dist * j);
				c->texture = tex;
				g->attach(c);
			}
			cube_groups.push_back(g);
			head->attach(g);
		}
	}

	auto cube = make_shared<Geometry>(n_shader, ParametricShapes::createCube(2.f,10));
	cube->translate(0.f, 0.f, -1.f);
	head->attach(cube);

	light->translate(-max*2*dist, 0, -max*2*dist);
	light_rotator->attach(light);
	head->attach(light_rotator);

	shared_ptr<Frame> f = make_shared<Frame>(l_shader);
	head->attach(f);
	
}
void ExampleWorld::update(double delta)
{
	active_camera->update(delta);
	light_rotator->world = glm::rotate(world_pos,(float) glfwGetTime()/10, { 1.f,0,-1.f });
	for (int i = 0; i < max; i++) {
		for (int j = 0; j < max; j++) {
			cube_groups[i*max + j]->world = glm::translate(world_pos, { 0,glm::sin(glfwGetTime() + i*glm::radians(offset) + j*glm::radians(offset)) , 0 });
		}
	}
}
