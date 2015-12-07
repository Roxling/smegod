#pragma once

#include "smegod_dep.h"
#include "shaders.h"

class BillboardList
{
public:
	BillboardList();
	~BillboardList();

	bool Init(const shared_ptr<Texture> tex);

	void Render(const glm::mat4 &VP, const glm::vec3 &CameraPos);

private:
	void CreatePositionBuffer();

	GLuint m_VA;
	GLuint m_VB;
	shared_ptr<Texture> m_pTexture;
	unique_ptr<ShaderGroup> m_technique;
	//BillboardTechnique m_technique;
};