#pragma once

#include "smegod_dep.h"
#include "core.h"
#include "shaders.h"

class Particles {
public:
	Particles(shared_ptr<ShaderGroup> m_updateShader, shared_ptr<ShaderGroup> m_renderShader);


	void swap();

	void update();
	void renderParticles();
	
private:

	GLuint m_particleBuffer[2];
	GLuint m_transformFeedback[2];
	GLuint m_vao[2];
	bool m_isFirst;
	unsigned int m_currVB;
	unsigned int m_currTFB;

	shared_ptr<ShaderGroup> m_updateShader;
	shared_ptr<ShaderGroup> m_renderShader;
};