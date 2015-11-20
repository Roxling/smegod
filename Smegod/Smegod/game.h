#pragma once
#include "smegod_dep.h"
#include "geometries.h"
#include "nodes.h"
#include "camera.h"
#include "input_handling.h"

/*
class Plane : public WorldObject {
private:
	shared_ptr<Geometry> propeller;
public:
	Plane(shared_ptr<ShaderGroup> mshader_group);
	void render(glm::mat4 combined_transform) override;
	void propell(double time);
};

class FlightCamera : public Camera {
public:
	FlightCamera(shared_ptr<WorldObject> mplane);
private:
	shared_ptr<WorldObject> plane;
	float maxspeed = 500;
	float minspeed = 50;
	float speed = minspeed;

	Coordinate mouseCenter;

protected:
	virtual void handleMouse(float delta) override;
	virtual void handleKeyboard(float delta) override;

};

class ConePair : public WorldObject {
private:
	shared_ptr<Geometry> cone1;
	shared_ptr<Geometry> cone2;

	float distance = 0;
	float height = 0;
public:
	ConePair(shared_ptr<ShaderGroup> mshader_group, float distance, float height);
	void setColor(float r, float g, float b);
	bool contains(glm::vec3 &pos);
};


void GenerateGameWorld(shared_ptr<Node> head, shared_ptr<ShaderGroup> shader_group);*/