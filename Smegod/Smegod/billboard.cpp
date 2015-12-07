#include "billboard.h"

#define NUM_ROWS 10
#define NUM_COLUMNS 10


BillboardList::BillboardList()
{
	m_VB = NULL;
}


BillboardList::~BillboardList()
{
	if (m_VB != NULL)
	{
		glDeleteBuffers(1, &m_VB);
	}
}


bool BillboardList::Init(const shared_ptr<Texture> tex)
{
	m_pTexture = tex;

	CreatePositionBuffer();

	m_technique = make_unique<ShaderGroup>("billboard.vert", "billboard.geom", "billboard.frag");

	return true;
}


typedef struct pos_t {
	GLfloat x, y, z;
} pos_t;

void BillboardList::CreatePositionBuffer()
{
	vector<pos_t> Positions;

	for (unsigned int j = 0; j < NUM_ROWS; j++) {
		for (unsigned int i = 0; i < NUM_COLUMNS; i++) {
			pos_t Pos;
			Pos.x = (float)i;
			Pos.y = 0.0f;
			Pos.z = (float)j;
			Positions.push_back(Pos);
		}
	}

	glGenVertexArrays(1, &m_VA);
	glGenBuffers(1, &m_VB);
	glBindVertexArray(m_VA);
	glBindBuffer(GL_ARRAY_BUFFER, m_VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_t) * Positions.size(), Positions.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(pos_t), (GLvoid*)0);

	glBindVertexArray(0);

	GL_CHECK_ERRORS_MSG("After Billboard init");
}


void BillboardList::Render(const glm::mat4 &view_projection, const glm::vec3 &camera_pos)
{
	m_technique->use();
	m_technique->setUniform("view_projection", view_projection);
	m_technique->setUniform("camera_pos", camera_pos);

	m_technique->bindTexture("tex", 0, *m_pTexture);

	glBindVertexArray(m_VA);
	GL_CHECK_ERRORS_MSG("Billboard render#3");
	glDrawArrays(GL_POINTS, 0, NUM_ROWS * NUM_COLUMNS);
	GL_CHECK_ERRORS_MSG("Billboard render#4");

	glBindVertexArray(0);
	GL_CHECK_ERRORS_MSG("After Billboard render");
}