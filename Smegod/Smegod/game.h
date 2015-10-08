#pragma once
#include "smegod_dep.h"
#include "geometries.h"
#include "nodes.h"
#include "camera.h"
#include "input_handling.h"


class Plane : public WorldObject {
private:
	shared_ptr<Geometry> propeller;
public:
	Plane(shared_ptr<ShaderGroup> mshader_group);
	void propell(double time);
};

class FlightCamera : public Camera {
private:
	float maxspeed = 500;
	float minspeed = 50;
	float speed = minspeed;

	Coordinate mouseCenter;

protected:
	void updateRotation(float pitch, float roll);
	virtual void handleMouse(float delta) override;
	virtual void handleKeyboard(float delta) override;
public:
	FlightCamera();
};

class ConePair : public WorldObject {
public:
	ConePair(shared_ptr<ShaderGroup> mshader_group, float distance, float height);
};