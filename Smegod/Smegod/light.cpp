#include "light.h"

Light::Light(shared_ptr<ShaderGroup> mshader_group) : Geometry(mshader_group)
{
	shared_ptr<Cube> source = make_shared<Cube>(mshader_group);
	source->world = glm::scale(source->world, glm::vec3(0.2f));
	source->color = glm::vec3(1.f);
	attach(source);
}
