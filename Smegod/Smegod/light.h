#pragma once
#include "smegod_dep.h"
#include "geometries.h"

class Light : public WorldObject, public ShaderCollection {
public:
	Light(shared_ptr<ShaderGroup>);
	void render(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader) override;
};


class SpotLight : public WorldObject {
public:

	SpotLight(shared_ptr<ShaderGroup> shader);

	void render(glm::mat4 combined_transform, shared_ptr<ShaderGroup> shader) override;
	
	glm::mat4 getLightSpaceMatrix();

	glm::vec3 LightColor = {.7,.7,.7};
	float LightIntensity = 24.0f;
private:
	void renderSelf();
	float LightAngleFalloff = 0.8f;

	GLuint coneVao = 0u, coneVbo = 0u;
	GLsizei coneVerticesNb = 0;
};