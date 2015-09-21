#include "world.h"
#include "static_data.h"
#include "shaders.h"
#include "geometries.h"
#include "textures.h"

World::World()
{
	head = make_shared<Node>();
}

vector<shared_ptr<Node>> cube_groups;
int max = 10;
float offset = 50.f;
float dist = 3.f;

void World::update(double delta)
{
	active_camera->update(delta);

	for (int i = 0; i < max; i++) {
		for (int j = 0; j < max; j++) {
			cube_groups[i*max+j]->world = glm::translate(world_pos, { 0,glm::sin(glfwGetTime() + i*glm::radians(offset) + j*glm::radians(offset)) , 0 } );
		}
	}
}

void World::render()
{
	head->render(world_pos);
}

void link_shader_program(GLuint shader_program) {
	glLinkProgram(shader_program);

	GLint success;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		const int logSize = 512;
		GLchar log[logSize];

		glGetProgramInfoLog(shader_program, logSize, NULL, log);
		cout << "Shader program failed to link." << endl << log << endl;
	}
	else {
		glUseProgram(shader_program);
	}

}



void ExampleWorld::initiate()
{
	active_shader_program = glCreateProgram();

	VertexShader projection = VertexShader("projection_vertex_shader.glsl");
	projection.compile();
	projection.attachTo(active_shader_program);


	/* PixelShader basic_pixel("basic_pixel_shader.glsl");
	basic_pixel.compile();
	basic_pixel.attachTo(active_shader_program); */

	PixelShader texture_pixel("texture_pixel_shader.glsl");
	texture_pixel.compile();
	texture_pixel.attachTo(active_shader_program);

	link_shader_program(active_shader_program); //only link when all shaders are created and attached.

	active_camera = make_shared<Camera>(45.f, WIDTH, HEIGHT, 0.1f, 100.f, active_shader_program);
	head->attach(active_camera);

	shared_ptr<Texture> tex = make_shared<Texture>("wood.jpg");


	for (int i = 0; i < max; i++) {
		for (int j = 0; j < max; j++) {
			shared_ptr<Node> g = make_shared<Node>();
			for (int k = 0; k < max; k++) {

				shared_ptr<Cube> c = make_shared<Cube>(active_shader_program);
				c->translate(dist*i, dist*k, dist * j);
				c->color = { (float)i / (float)max , (float)k / (float)max, (float)j / (float)max };
				c->texture = tex;
				g->attach(c);
			}
			cube_groups.push_back(g);
			head->attach(g);
		}
	}

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
