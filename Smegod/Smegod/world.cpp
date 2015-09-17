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

void World::update()
{
}

void World::render()
{
	active_camera->render();
	head->render(active_shader_program);
}

void World::initiate()
{
	active_shader_program = glCreateProgram();
	setActiveCamera(make_shared<Camera>(45.f, WIDTH, HEIGHT, 0.1f, 100.f));
	active_camera->setupShader(active_shader_program);

	PixelShader basic_pixel("basic_pixel_shader.glsl");
	basic_pixel.compile();
	basic_pixel.attachTo(active_shader_program);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {

				shared_ptr<Cube> c = make_shared<Cube>();
				c->translate(1.5f*i, 1.5f*j, 1.5f*k);
				head->attach(c);
			}
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

void Node::render(GLuint shader_program)
{
	GLint world_location = glGetUniformLocation(shader_program, "world");
	glUniformMatrix4fv(world_location, 1, GL_FALSE, glm::value_ptr(world));
	renderSelf();
	for (auto it = children.begin(); it != children.end(); ++it) {
		(*it)->render(shader_program);
	}
}