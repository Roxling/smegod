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
	void Particles::Render(const glm::mat4 &view_projection, const glm::vec3 &camera_pos, shared_ptr<Texture> m_pTexture);

private:

	GLuint m_particleBuffer[2];
	GLuint m_transformFeedback[2];
	GLuint m_vao[2];
	bool m_isFirst;
	unsigned int m_currVB;
	unsigned int m_currTFB;

	GLuint m_query = 0;

	shared_ptr<ShaderGroup> m_updateShader;
	shared_ptr<ShaderGroup> m_renderShader;
	shared_ptr<ShaderGroup> m_billboardShader;
};