#pragma once
#include "smegod_dep.h"
#include "geometries.h"
#include "nodes.h"
#include "camera.h"


class Plane : public WorldObject {
private:
	shared_ptr<Geometry> propeller;
public:
	Plane(shared_ptr<ShaderGroup> mshader_group);
	void propell(double time);
};

class FlightCamera : public Camera {
private:
	float speed = 0;

protected:
	void updateRotation(float pitch, float roll);
	virtual void handleMouse(float delta) override;
	virtual void handleKeyboard(float delta) override;
public:
	FlightCamera();
};