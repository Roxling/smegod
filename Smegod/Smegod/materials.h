#pragma once
#include "smegod_dep.h"

class Material {
public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat shininess;
	Material(glm::vec3 nambient, glm::vec3 ndiffuse, glm::vec3 nspecular, GLfloat nshininess)
		: ambient(nambient), diffuse(ndiffuse), specular(nspecular), shininess(nshininess) {}

	static Material DEFAULT;
};