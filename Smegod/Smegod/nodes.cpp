#include "nodes.h"

void Node::translate(float dx, float dy, float dz)
{
	world = glm::translate(world, { dx, dy, dz });
}

void Node::setTranslate(float x, float y, float z)
{
	world = glm::mat4(world[0], world[1], world[2], glm::vec4(x, y, z, 1));
}

void Node::rotate(float dyaw, float dpitch, float droll)
{
	auto rot = glm::mat4(glm::quat({ glm::radians(-dpitch),{ glm::radians(dyaw) },glm::radians(-droll) }));
	world = world * rot;
}

void Node::scale(float x, float y, float z)
{
	world = glm::scale(world, glm::vec3(x, y, z));
}
void Node::scale(float factor)
{
	scale(factor, factor, factor);
}

glm::vec3 Node::getPosition() {
	return glm::vec3(combined_world[3]);
}



void Node::attach(shared_ptr<Node> child)
{
	children.push_back(child);
}

void Node::render(glm::mat4 combined_transforms, shared_ptr<ShaderGroup> shader)
{
	combined_world = combined_transforms * world;
	renderSelf(combined_world, shader);
	for (auto it = children.begin(); it != children.end(); ++it) {
		(*it)->render(combined_world, shader);
	}
}

void WorldObject::renderSelf(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader)
{
	if (shader != nullptr) {
		world_location = glGetUniformLocation(shader->getGlId(), "world");
		worldIT_location = glGetUniformLocation(shader->getGlId(), "worldIT");
	}
	render(combined_transform, shader);
}

