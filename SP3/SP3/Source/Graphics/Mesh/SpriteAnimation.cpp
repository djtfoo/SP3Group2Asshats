#include "SpriteAnimation.h"
#include "GL\glew.h"
#include "../../Base/Source/Graphics/Vertex/Vertex.h"

SpriteAnimation::SpriteAnimation(const std::string &meshName, int row, int col):
Mesh(meshName)
, m_row(row)
, m_col(col)
, m_currentTime(0)
, m_currentFrame(0)
, m_playCount(0)
{
	m_anim = NULL;
}

SpriteAnimation::~SpriteAnimation()
{
	if (m_anim)
		delete m_anim;
}

void SpriteAnimation::Update(double dt)
{
	if (m_anim->animActive == true)
	{
		//Get Animation current time
		m_currentTime += static_cast<float>(dt);

		//find numFrame
		int numFrame = Math::Max(1, m_anim->endFrame - m_anim->startFrame + 1);

		//get frameTime
		float frameTime = m_anim->animTime / numFrame;

		//Get animation current frame, m_currentFrame
		m_currentFrame = Math::Min(m_anim->endFrame, m_anim->startFrame + static_cast<int>(m_currentTime / frameTime));

		if (m_currentTime >= m_anim->animTime)
		{
			if (m_anim->repeatCount == 0)
			{
				m_anim->animActive = false;
				m_currentTime = 0.f;
				m_currentFrame = m_anim->startFrame;
			}
			if (m_anim->repeatCount == 1)
			{
				m_currentTime = 0.f;
				m_currentFrame = m_anim->startFrame;
			}
		}
	}
}

void SpriteAnimation::Render()
{
	// Do more MAGIC!!!!!!!
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Position));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Position)+sizeof(Color)));
	//if (textureID > 0)
	{
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Position)+sizeof(Color)+sizeof(Vector3)));
	}

	//glDrawArrays(GL_TRIANGLES, offset, count);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	if (mode == DRAW_LINES)
		glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, (void*)(m_currentFrame * 6 * sizeof(GLuint)));
	else if (mode == DRAW_TRIANGLE_STRIP)
		glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, (void*)(m_currentFrame * 6 * sizeof(GLuint)));
	else
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(m_currentFrame * 6 * sizeof(GLuint)));

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	if (textureID > 0)
	{
		glDisableVertexAttribArray(3);
	}
}