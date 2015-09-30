#pragma once
#include "smegod_dep.h"
#include "geometries.h"

class Light : public WorldObject, public ShaderCollection {
public:
	Light(shared_ptr<ShaderGroup>);
	void render(glm::mat4 combined_transform) override;
};
