#include "particles.h"

typedef struct particle_t
{
	GLfloat pos[3];
	glm::vec3 seed;
	glm::vec3 speed;
	GLfloat random;
	GLuint type;
} particle_t;

#define NUM_RAIN (50000)

#define random() ((float)rand()/(float)RAND_MAX)

Particles::Particles() {
	m_currVB = 0;
	m_currTFB = 1;
	m_isFirst = true;

	float g_radiusRange = 20;
	float g_heightRange = 10;

	//TODO: Init data
	vector<particle_t> particles;

	for (int i = 0; i < NUM_RAIN; i++)
	{
		particle_t pt;
		particles.push_back(pt);
		float SeedX;
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
		float SeedY = random()*g_heightRange;
		particles[i].seed = glm::vec3(SeedX, SeedY, SeedZ);

		//add some random speed to the particles, to prevent all the particles from following exactly the same trajectory
		//additionally, random speeds in the vertical direction ensure that temporal aliasing is minimized
		float SpeedX = 40.0f*(random() / 20.0f);
		float SpeedZ = 40.0f*(random() / 20.0f);
		float SpeedY = 40.0f*(random() / 10.0f);
		particles[i].speed = glm::vec3(SpeedX, SpeedY, SpeedZ);

		//move the rain particles to a random positions in a cylinder above the camera
		float x = SeedX;// +g_vecEye.x;
		float z = SeedZ;// +g_vecEye.z;
		float y = SeedY;// +g_vecEye.y;
		particles[i].pos[0] = x;
		particles[i].pos[0] = y;
		particles[i].pos[0] = z;

		//get an integer between 1 and 8 inclusive to decide which of the 8 types of rain textures the particle will use
		particles[i].type = int(floor(random() * 8 + 1));

		//this number is used to randomly increase the brightness of some rain particles
		particles[i].random = 1;
		float randomIncrease = random();
		if (randomIncrease > 0.8)
			particles[i].random += randomIncrease;
	}

	glGenTransformFeedbacks(2, m_transformFeedback);
	glGenBuffers(2, m_particleBuffer);
	GL_CHECK_ERRORS();

	for (unsigned int i = 0; i < 2; i++) {
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particle_t) * particles.size(), particles.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]);
	}
	GL_CHECK_ERRORS();
	glGenVertexArrays(1, &vao);

}

void Particles::swap()
{
	m_currVB = m_currTFB;
	m_currTFB = (m_currTFB + 1) & 0x1;
}

void Particles::update()
{

	glEnable(GL_RASTERIZER_DISCARD);
	
	 //throws heaps of errors if no VAO is created..
	glBindVertexArray(vao);

	GL_CHECK_ERRORS_MSG("Particles update 1");
	glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currVB]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[m_currTFB]);
	GL_CHECK_ERRORS_MSG("Particles update 2");
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	GL_CHECK_ERRORS_MSG("Particles update 3");
	glVertexAttribPointer(0, 3, GL_FLOAT,        GL_FALSE, sizeof(particle_t), 0);                                     // vec3 pos
	GL_CHECK_ERRORS_MSG("Particles update 3.1");
	glVertexAttribPointer(1, 3, GL_FLOAT,        GL_FALSE, sizeof(particle_t), (const GLvoid*)(3 * sizeof GLfloat));   // vec3 seed
	GL_CHECK_ERRORS_MSG("Particles update 3.2");
	glVertexAttribPointer(2, 3, GL_FLOAT,        GL_FALSE, sizeof(particle_t), (const GLvoid*)(6 * sizeof GLfloat));   // vec3 speed
	GL_CHECK_ERRORS_MSG("Particles update 3.3");
	glVertexAttribPointer(3, 1, GL_FLOAT,        GL_FALSE, sizeof(particle_t), (const GLvoid*)(9 * sizeof GLfloat));   // float random
	GL_CHECK_ERRORS_MSG("Particles update 3.4");
	glVertexAttribPointer(4, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(particle_t), (const GLvoid*)(10 * sizeof GLfloat));  // unsigned int type
	GL_CHECK_ERRORS_MSG("Particles update 4");
	glBeginTransformFeedback(GL_POINTS);
	GL_CHECK_ERRORS_MSG("Particles update 5");
	if (m_isFirst) {
		glDrawArrays(GL_POINTS, 0, NUM_RAIN);
		m_isFirst = false;
		GL_CHECK_ERRORS_MSG("Particles update 6");
	}
	else {
		glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currVB]);
		GL_CHECK_ERRORS_MSG("Particles update 7");
	}

	glEndTransformFeedback();
	GL_CHECK_ERRORS_MSG("Particles update 8");

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	GL_CHECK_ERRORS_MSG("Particles update 9");
	glFlush();
}

void Particles::renderParticles()
{
	glDisable(GL_RASTERIZER_DISCARD);
	//glBindVertexArray(vao);
	GL_CHECK_ERRORS_MSG("Particles render 1");
	glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currTFB]);
	GL_CHECK_ERRORS_MSG("Particles render 2");

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	GL_CHECK_ERRORS_MSG("Particles render 3");

	glVertexAttribPointer(0, 3, GL_FLOAT,        GL_FALSE, sizeof(particle_t), 0);                                     // vec3 pos
	glVertexAttribPointer(1, 3, GL_FLOAT,        GL_FALSE, sizeof(particle_t), (const GLvoid*)(3 * sizeof GLfloat));   // vec3 seed
	glVertexAttribPointer(2, 3, GL_FLOAT,        GL_FALSE, sizeof(particle_t), (const GLvoid*)(6 * sizeof GLfloat));   // vec3 speed
	glVertexAttribPointer(3, 1, GL_FLOAT,        GL_FALSE, sizeof(particle_t), (const GLvoid*)(9 * sizeof GLfloat));   // float random
	glVertexAttribPointer(4, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(particle_t), (const GLvoid*)(10 * sizeof GLfloat));  // unsigned int type
	GL_CHECK_ERRORS_MSG("Particles render 4");

	glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currTFB]);

	GL_CHECK_ERRORS_MSG("Particles render 5");

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	GL_CHECK_ERRORS_MSG("Particles render 6");

}
