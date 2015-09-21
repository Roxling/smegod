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

WorldObject::WorldObject(GLuint mshader_program) : shader_program(mshader_program)
{
	world_location = glGetUniformLocation(shader_program, "world");
}

void WorldObject::renderSelf(glm::mat4 combined_transform)
{
	glUseProgram(shader_program);
	glUniformMatrix4fv(world_location, 1, GL_FALSE, glm::value_ptr(combined_transform));
	render();
}