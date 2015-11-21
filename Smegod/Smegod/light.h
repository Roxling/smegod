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
	
private:
	void renderSelf();
	glm::vec3 LightColor = {.7,.7,.7};
	float LightIntensity = 240000.0f;
	float LightAngleFalloff = 0.8f;
	glm::vec2 ShadowMapTexelSize = { 1.0f / 512.f, 1.0f / 512.f };
	glm::vec2 invRes = { 1.0f / Globals::WIDTH,1.0f / Globals::HEIGHT};

	GLuint coneVao = 0u, coneVbo = 0u;
	GLsizei coneVerticesNb = 0;
};