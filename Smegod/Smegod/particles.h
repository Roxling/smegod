#pragma once

#include "smegod_dep.h"
#include "core.h"

class Particles {
public:
	Particles();

private:
	GLuint m_particleBuffer[2];
	GLuint m_transformFeedback[2];
};