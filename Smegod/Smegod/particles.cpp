#include "particles.h"

typedef struct particle_t
{
	glm::vec3 pos;
	glm::vec3 seed;
	glm::vec3 speed;
	GLfloat random;
	GLfloat type;
} particle_t;

#define NUM_RAIN (1000000)



Particles::Particles(shared_ptr<ShaderGroup> m_updateShader, shared_ptr<ShaderGroup> m_renderShader) {
	this->m_updateShader = m_updateShader;
	this->m_renderShader = m_renderShader;
	m_currVB = 0;
	m_currTFB = 1;
	m_isFirst = true;

	float g_radiusRange = 150;
	float g_heightRange = 35;

	vector<particle_t> particles;

	for (int i = 0; i < NUM_RAIN; i++)
	{
		particle_t pt;
		particles.push_back(pt);
		float SeedX;
		float SeedY = random()*g_heightRange;
		float SeedZ;

		bool pointIsInside = false;
		while (!pointIsInside)
		{
			SeedX = random() - 0.5f;
			SeedZ = random() - 0.5f;
			if (sqrt(SeedX*SeedX + SeedZ*SeedZ) <= 0.5f)
				pointIsInside = true;
		}
		//save these random locations for reinitializing rain particles that have fallen out of bounds
		SeedX *= g_radiusRange;
		SeedZ *= g_radiusRange;
		
		particles[i].seed = glm::vec3(SeedX, SeedY, SeedZ);

		float SpeedX = 40.0f*(random() / 10.0f);
		float SpeedZ = 40.0f*(random() / -10.0f);
		float SpeedY = -40.0f*(random() / 10.0f);

		particles[i].speed = glm::vec3(SpeedX, SpeedY, SpeedZ);
		particles[i].pos = glm::vec3(SeedX, SeedY, SeedZ);
		particles[i].type = floor(random() * 6 + 1);
		particles[i].random = 0.5f + random()*0.5f;
	}

	glGenVertexArrays(2, m_vao);
	glGenTransformFeedbacks(2, m_transformFeedback);
	glGenBuffers(2, m_particleBuffer);
	GL_CHECK_ERRORS();

	for (unsigned int i = 0; i < 2; i++) {
		//init buffers
		glBindVertexArray(m_vao[i]);
			glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[i]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(particle_t) * particles.size(), particles.data(), GL_DYNAMIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(particle_t), 0);                                     // vec3 pos
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(particle_t), (const GLvoid*)(3 * sizeof GLfloat));   // vec3 seed
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(particle_t), (const GLvoid*)(6 * sizeof GLfloat));   // vec3 speed
				glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(particle_t), (const GLvoid*)(9 * sizeof GLfloat));   // float random
				glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(particle_t), (const GLvoid*)(10 * sizeof GLfloat));  // int type
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glBindVertexArray(0);

		//bind feedback buffers
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	}
	GL_CHECK_ERRORS();
}

void Particles::swap()
{
	m_currVB = m_currTFB;
	m_currTFB = (m_currTFB + 1) & 0x1;
}

void Particles::update()
{
	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(m_vao[m_currVB]);
	GL_CHECK_ERRORS_MSG("Particles update 1");

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[m_currTFB]);
	GL_CHECK_ERRORS_MSG("Particles update 2");

	glBeginTransformFeedback(GL_POINTS);

	if (m_isFirst) {
		m_isFirst = false;
		glDrawArrays(GL_POINTS, 0, NUM_RAIN);
		GL_CHECK_ERRORS_MSG("Particles update 6");
	}
	else {
		glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currVB]);
		GL_CHECK_ERRORS_MSG("Particles update 7");
	}

	glEndTransformFeedback();
	GL_CHECK_ERRORS_MSG("Particles update 8");
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

	glBindVertexArray(0);

	glDisable(GL_RASTERIZER_DISCARD);
}

void Particles::renderParticles()
{
	glBindVertexArray(m_vao[m_currTFB]);
	GL_CHECK_ERRORS_MSG("Billboard render#3");
	glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currTFB]);
	GL_CHECK_ERRORS_MSG("Billboard render#4");

	glBindVertexArray(0);
	GL_CHECK_ERRORS_MSG("After Billboard render");
}
