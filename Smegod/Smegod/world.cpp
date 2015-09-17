#include "world.h"

World::World()
{
	head = make_shared<Group>();
}

void World::setActiveCamera(shared_ptr<Camera>& cam)
{
	active_camera = cam;
}

void World::update()
{
}

void World::render()
{
}

void Node::setTranslation(float x, float y, float z)
{
	//TODO: make Nodes moveable
}