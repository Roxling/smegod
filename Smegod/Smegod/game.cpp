#include "game.h"

Plane::Plane(shared_ptr<ShaderGroup> mshader_group) :WorldObject(mshader_group)
{
	shared_ptr<Texture> tex = make_shared<Texture>("stone43_diffuse.jpg");
	shared_ptr<Texture> wingtex = make_shared<Texture>("game/wing.jpg");
	shared_ptr<Texture> wheeltex = make_shared<Texture>("game/wheel.jpg");
	shared_ptr<Texture> bodytex = make_shared<Texture>("game/body.jpg");

	shared_ptr<Texture> bump = make_shared<Texture>("stone43_bump.jpg");
	glm::vec3 ambient(0.4f);
	glm::vec3 diffuse(0.8f);
	glm::vec3 spefular(1.f);
	GLfloat shininess = 50;
	Material mat(ambient, diffuse, spefular, shininess);

	auto wing = make_shared<Geometry>(mshader_group, ParametricShapes::createBlock(8,0.2f,30,1));
	wing->translate(2, -2, 15);
	wing->texture = wingtex;
	wing->bumpmap = bump;
	wing->material = mat;
	attach(wing);

	float x = 1;
	float z = -3;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			auto pin = make_shared<Geometry>(mshader_group, ParametricShapes::createBlock(0.4f, 5, 0.4f, 1));
			pin->texture = tex;
			pin->bumpmap = bump;
			pin->material = mat;
			pin->translate(x, 0, z);
			wing->attach(pin);
			x = 7.6f;
		}
		x = 1;
		z = -27;
	}
	float rad = 1;
	float y = 1.9f;
	x = 1;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			auto pin = make_shared<Geometry>(mshader_group, ParametricShapes::createBlock(0.4f, 5.4f, 0.4f, 1));
			pin->texture = tex;
			pin->bumpmap = bump;
			pin->material = mat;
			pin->translate(x, y, -15);
			pin->world = glm::rotate(pin->world, rad, { world[0].x,world[0].y,world[0].z });
			wing->attach(pin);
			x = 7;
		}
		x = 0;
		y = 2.2f;
		rad = -1;
	}

	z = -13;
	rad = -0.3f;
	y = -2;
	for (int j = 0; j < 2; j++) {
		auto pin = make_shared<Geometry>(mshader_group, ParametricShapes::createBlock(0.4f, 4.5f, 0.4f, 1));
		pin->texture = tex;
		pin->bumpmap = bump;
		pin->material = mat;
		pin->translate(8, y, z);
		pin->world = glm::rotate(pin->world, rad, { world[0].x,world[0].y,world[0].z });
		wing->attach(pin);
		auto wheel = make_shared<Geometry>(mshader_group, ParametricShapes::createTorus(0.7f, 1.f, 10, 10));
		wheel->texture = wheeltex;
		wheel->bumpmap = bump;
		wheel->material = mat;
		wheel->translate(8, -2, z);
		wheel->world = glm::scale(wheel->world, glm::vec3(1, 1, 0.3f));
		wing->attach(wheel);
		rad = 0.3f;
		y = -2.2f;
		z = -16.1f;
	}

	auto back = make_shared<Geometry>(mshader_group, ParametricShapes::createSphere(2.5f,10,10));
	back->texture = wingtex;
	back->bumpmap = bump;
	back->material = mat;
	back->translate(-15, 4, -15);
	back->world = glm::scale(back->world, glm::vec3(0.7f, 1.f, 0.2f));
	wing->attach(back);

	auto smallwing = make_shared<Geometry>(mshader_group, ParametricShapes::createSphere(3, 10, 10));
	smallwing->texture = wingtex;
	smallwing->bumpmap = bump;
	smallwing->material = mat;
	smallwing->translate(-15, 2, -15);
	smallwing->world = glm::scale(smallwing->world, glm::vec3(0.5f, 0.1f, 1.5));
	wing->attach(smallwing);

	auto wheel = make_shared<Geometry>(mshader_group, ParametricShapes::createSphere(0.8f,10,10));
	wheel->texture = wheeltex;
	wheel->bumpmap = bump;
	wheel->material = mat;
	wheel->translate(-15, 0.1f, -15);
	wheel->world = glm::scale(wheel->world, glm::vec3(1, 1, 0.3f));
	wing->attach(wheel);


	auto wing2 = make_shared<Geometry>(mshader_group, ParametricShapes::createBlock(8, 0.2f, 30, 1));
	wing2->translate(0, 5, 0);
	wing2->texture = wingtex;
	wing2->bumpmap = bump;
	wing2->material = mat;
	wing->attach(wing2);
	auto body = make_shared<Geometry>(mshader_group, ParametricShapes::createSphere(2,20,20));
	body->world = glm::scale(body->world, glm::vec3(8,1,1));
	body->texture = bodytex;
	body->bumpmap = bump;
	body->material = mat;
	attach(body);

	auto nose = make_shared<Geometry>(mshader_group, ParametricShapes::createSphere(0.5f,10,10));
	nose->translate(13.7f, 2, -15);
	nose->world = glm::scale(nose->world, glm::vec3(0.6f, 1, 1));
	nose->texture = wheeltex;
	nose->bumpmap = bump;
	nose->material = mat;
	wing->attach(nose);


	propeller = make_shared<Geometry>(mshader_group, ParametricShapes::createBlock(0.1f, 5.f, 0.5f, 1));
	propeller->translate(13.8f, 2, -15);
	propeller->texture = wheeltex;
	propeller->bumpmap = bump;
	propeller->material = mat;
	wing->attach(propeller);

	auto propeller2 = make_shared<Geometry>(mshader_group, ParametricShapes::createBlock(0.1f, 5.f, 0.5f, 1));
	propeller2->translate(0,-5,0);
	propeller2->texture = wheeltex;
	propeller2->bumpmap = bump;
	propeller2->material = mat;
	propeller->attach(propeller2);
}

vector<shared_ptr<ConePair>> coneList;
shared_ptr<ConePair> nextCone;
bool first = true;
int index = 0;

void Plane::render(glm::mat4 combined_transform)
{
	if (first) {
		first = false;
		if (index < coneList.size()) {
			nextCone = coneList[index++];
			nextCone->setColor(0, 1, 0);
		}
	}
	glm::vec3 pos(combined_transform[3]);
	if (nextCone != nullptr && nextCone->contains(pos)) {
		if (index < coneList.size()) {
			nextCone->setColor(0, 0, 1);
			nextCone = coneList[index++];
			nextCone->setColor(0, 1, 0);
		}
		else {
			nextCone->setColor(0, 0, 1);
			nextCone = nullptr;
			for (int i = 0; i < 1000; i++)
				cout << "GAME FINISHED" << endl;
		}
	}

}

void Plane::propell(double d)
{
	float delta = (float) d;
	//translate(delta * 100, 0, 0);
	propeller->world = glm::rotate(propeller->world, 100* delta, { propeller->world[0].x,propeller->world[0].y,propeller->world[0].z });
}

FlightCamera::FlightCamera(shared_ptr<WorldObject> mplane) : plane(mplane), Camera(45.f, Globals::WIDTH, Globals::HEIGHT, 0.1f, 500000), mouseCenter(Coordinate(0.0, 0.0))
{
	translate(0, 10, -50);
	rotate(180, 0, 0);
}


void FlightCamera::handleMouse(float delta)
{
	auto takingCursorInput = InputHandler::getMouseButtonstate(GLFW_MOUSE_BUTTON_RIGHT) != GLFW_RELEASE;
	if (takingCursorInput != oldTakingCursorInput) {
		oldTakingCursorInput = takingCursorInput;
		mouseCenter = InputHandler::getMousePos();
		glfwSetInputMode(InputHandler::active_window, GLFW_CURSOR, takingCursorInput ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}
	if (takingCursorInput) {
		auto coord = InputHandler::getMousePos() - mouseCenter;
		
		coord.x *= 0.0002;
		coord.y *= 0.0002;
		plane->rotate(0, (float)-coord.x, (float)-coord.y);
	}
}

void FlightCamera::handleKeyboard(float delta)
{
	int speedUp = InputHandler::getKeystate(GLFW_KEY_W) != GLFW_RELEASE ? 1 : 0;
	int speedDown = InputHandler::getKeystate(GLFW_KEY_S) != GLFW_RELEASE ? -1 : 0;

	float trans_factor = 100 * delta;

	speed += (speedUp + speedDown) * trans_factor;

	if (speed < minspeed)
		speed = minspeed;
	else if (speed > maxspeed)
		speed = maxspeed;

	plane->translate(speed*delta, 0, 0);

}


ConePair::ConePair(shared_ptr<ShaderGroup> mshader_group, float distance, float height) : WorldObject(mshader_group)
{
	this->height = height;
	this->distance = distance;
	float radius = height / 20.f;
	int res = 20;

	cone1 = make_shared<Geometry>(mshader_group, ParametricShapes::createCone(radius, height, res));
	cone1->color = { 1,0,0 };
	cone1->translate(0, 0, -distance / 2);
	attach(cone1);

	cone2 = make_shared<Geometry>(mshader_group, ParametricShapes::createCone(radius, height, res));
	cone2->color = { 1,0,0 };
	cone2->translate(0, 0, distance / 2);
	attach(cone2);
}

void ConePair::setColor(float r, float g, float b)
{
	auto color = glm::vec3(r, g, b);
	cone1->color = color;
	cone2->color = color;
}

bool ConePair::contains(glm::vec3 &pos)
{
	auto mypos = glm::vec3(world[3]);
	return	(glm::abs(pos.x - mypos.x)) < 10 &&
			(pos.y < height && pos.y > 0)  &&
			(glm::abs(pos.z - mypos.z)) < distance/2;

}



void GenerateGameWorld(shared_ptr<Node> head, shared_ptr<ShaderGroup> shader_group) {

	auto pair = make_shared<ConePair>(shader_group, 150.f, 500.f);
	head->attach(pair);
	pair->translate(-500, 0, 300);
	coneList.push_back(pair);

	pair = make_shared<ConePair>(shader_group, 150.f, 500.f);
	head->attach(pair);
	pair->translate(-1000, 0, 0);
	coneList.push_back(pair);

	pair = make_shared<ConePair>(shader_group, 150.f, 500.f);
	head->attach(pair);
	pair->translate(-1500, 0, 300);
	coneList.push_back(pair);

	pair = make_shared<ConePair>(shader_group, 150.f, 500.f);
	head->attach(pair);
	pair->translate(-2000, 0, 0);
	coneList.push_back(pair);

	pair = make_shared<ConePair>(shader_group, 150.f, 500.f);
	head->attach(pair);
	pair->translate(-2500, 0, 300);
	coneList.push_back(pair);

	pair = make_shared<ConePair>(shader_group, 150.f, 500.f);
	head->attach(pair);
	pair->translate(-3000, 0, 0);
	coneList.push_back(pair);

	pair = make_shared<ConePair>(shader_group, 150.f, 500.f);
	head->attach(pair);
	pair->translate(-3500, 0, 300);
	coneList.push_back(pair);
	
}
