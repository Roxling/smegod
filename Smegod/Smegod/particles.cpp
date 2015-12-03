#include "particles.h"

typedef struct particle_t
{
	float type;
	glm::vec3 pos;
	glm::vec3 vel;
	float lifetimeMillis;
} particle_t;


Particles::Particles() {
	particle_t particles[128];
	glGenTransformFeedbacks(2, m_transformFeedback);
	glGenBuffers(2, m_particleBuffer);
	GL_CHECK_ERRORS();

	for (unsigned int i = 0; i < 2; i++) {
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]);
	}
	GL_CHECK_ERRORS();


}