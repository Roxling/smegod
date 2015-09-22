#pragma once
#include "smegod_dep.h"
#include "geometries.h"

class Light : public Geometry, public ShaderCollection {
public:
	Light(shared_ptr<ShaderGroup>);

	void render() override;
};
