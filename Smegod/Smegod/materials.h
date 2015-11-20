#pragma once
#include "smegod_dep.h"

class Material {
public:
	vector<pair<string, GLuint>> textures;
	glm::vec3 color = { 1.f,1.f,1.f };
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat shininess;
	Material() : ambient(glm::vec3(.2f)), diffuse(glm::vec3(.8f)), specular(glm::vec3(1.f)), shininess(10) {}
	Material(glm::vec3 nambient, glm::vec3 ndiffuse, glm::vec3 nspecular, GLfloat nshininess)
		: ambient(nambient), diffuse(ndiffuse), specular(nspecular), shininess(nshininess) {}

	static Material DEFAULT;
};