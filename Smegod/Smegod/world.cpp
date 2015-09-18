#include "world.h"
#include "static_data.h"
#include "shaders.h"
#include "geometries.h"

World::World()
{
	head = make_shared<Group>();
}

void World::setActiveCamera(shared_ptr<Camera> cam)
{
	active_camera = cam;
}

vector<shared_ptr<Group>> cube_groups;
int max = 10;
float offset = 50.f;
float dist = 3.f;

void World::update()
{
	
	for (int i = 0; i < max; i++) {
		for (int j = 0; j < max; j++) {
			cube_groups[i*max+j]->world = glm::translate(world_pos, { 0,glm::sin(glfwGetTime() + i*glm::radians(offset) + j*glm::radians(offset)) , 0 } );
		}
	}
}

void World::render()
{
	active_camera->render();
	
	head->render(world_pos);
}

void World::initiate()
{
	active_shader_program = glCreateProgram();
	setActiveCamera(make_shared<Camera>(45.f, WIDTH, HEIGHT, 0.1f, 100.f));
	active_camera->setupShader(active_shader_program);

	PixelShader basic_pixel("basic_pixel_shader.glsl");
	basic_pixel.compile();
	basic_pixel.attachTo(active_shader_program);

	for (int i = 0; i < max; i++) {
		for (int j = 0; j < max; j++) {
			shared_ptr<Group> g = make_shared<Group>();
			for (int k = 0; k < max; k++) {
				
				shared_ptr<Cube> c = make_shared<Cube>(active_shader_program);
				c->translate(dist*i, dist*k, dist * j);
				c->color = { (float)i / (float) max , (float)k / (float)max, (float)j / (float)max };
				g->attach(c);
			}
			cube_groups.push_back(g);
			head->attach(g);
		}
	}

}

void Node::translate(float dx, float dy, float dz)
{
	world = glm::translate(world,{ dx, dy, dz });
}

void Node::attach(shared_ptr<Node> child)
{
	children.push_back(child);
}

void Node::render(glm::mat4 combined_transforms)
{
	glm::mat4 result = combined_transforms * world;
	renderSelf(result);
	for (auto it = children.begin(); it != children.end(); ++it) {
		(*it)->render(result);
	}
}