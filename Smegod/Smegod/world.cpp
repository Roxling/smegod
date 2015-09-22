#include "world.h"
#include "static_data.h"
#include "shaders.h"
#include "geometries.h"
#include "textures.h"
#include "light.h"

World::World()
{
	head = make_shared<Node>();
}

void World::render()
{
	head->render(world_pos);
}

vector<shared_ptr<Node>> cube_groups;
int max = 10;
float offset = 50.f;
float dist = 3.f;

void ExampleWorld::initiate()
{
	active_camera = make_shared<Camera>(45.f, WIDTH, HEIGHT, 0.1f, 100.f);
	head->attach(active_camera);

	
	shared_ptr<ShaderGroup> n_shader = make_shared<ShaderGroup>("projection.vs", "texture.fs");
	active_camera->addShaderGroup(n_shader);
	
	shared_ptr<Texture> tex = make_shared<Texture>("wood.jpg");

	for (int i = 0; i < max; i++) {
		for (int j = 0; j < max; j++) {
			shared_ptr<Node> g = make_shared<Node>();
			for (int k = 0; k < max; k++) {

				shared_ptr<Cube> c = make_shared<Cube>(n_shader);
				c->translate(dist*i, dist*k, dist * j);
				c->color = { (float)i / (float)max , (float)k / (float)max, (float)j / (float)max };
				c->texture = tex;
				g->attach(c);
			}
			cube_groups.push_back(g);
			head->attach(g);
		}
	}
	//cube_groups[0]->attach(active_camera);
	shared_ptr<ShaderGroup> l_shader = make_shared<ShaderGroup>("light.vs", "light.fs");
	active_camera->addShaderGroup(l_shader);

	shared_ptr<Light> light = make_shared<Light>(l_shader);
	light->translate(-3.f, 0.f, -3.f);
	head->attach(light);

	shared_ptr<Frame> f = make_shared<Frame>(l_shader);
	head->attach(f);
	
}
void ExampleWorld::update(double delta)
{
	active_camera->update(delta);
	for (int i = 0; i < max; i++) {
		for (int j = 0; j < max; j++) {
			cube_groups[i*max + j]->world = glm::translate(world_pos, { 0,glm::sin(glfwGetTime() + i*glm::radians(offset) + j*glm::radians(offset)) , 0 });
		}
	}
}
