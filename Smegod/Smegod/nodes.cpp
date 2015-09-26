#include "nodes.h"

void Node::translate(float dx, float dy, float dz)
{
	world = glm::translate(world, { dx, dy, dz });
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

void WorldObject::renderSelf(glm::mat4 combined_transform)
{
	world_location = glGetUniformLocation(shader_group->getProgram(), "world");
	worldIT_location = glGetUniformLocation(shader_group->getProgram(), "worldIT");
	render(combined_transform);
}

