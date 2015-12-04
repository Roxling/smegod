#pragma once

#include "smegod_dep.h"
#include "core.h"

class Particles {
public:
	Particles();


	void render();

	void update();
	void renderParticles();

private:
	GLuint m_particleBuffer[2];
	GLuint m_transformFeedback[2];

	bool m_isFirst;
	unsigned int m_currVB;
	unsigned int m_currTFB;
};