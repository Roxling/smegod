#include "world.h"
#include "static_data.h"

World::World()
{
	head = make_shared<Group>();
}

void World::setActiveCamera(shared_ptr<Camera> cam)
{
	active_camera = cam;
}

void World::update()
{
}

void World::render()
{
	active_camera->render();
}

void World::initiate()
{
	active_shader_program = glCreateProgram();
	setActiveCamera(make_shared<Camera>(45.f, WIDTH, HEIGHT, 0.1f, 100.f));
	active_camera->setupShader(active_shader_program);
}

void Node::setTranslation(float x, float y, float z)
{
	//TODO: make Nodes moveable
}