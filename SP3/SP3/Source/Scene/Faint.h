#ifndef FAINT_H
#define FAINT_H

#include "Scene.h"

class Faint
{

public:
	Scene* scene; // to render on top

	Faint();
	~Faint();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	bool b_mouseClick;

	float m_worldHeight;
	float m_worldWidth;
	
	bool b_playOnes;
	bool b_DeathSound;
	bool button_highlighted;

	void RenderFaintMenuState();

	void FaintMenuButton();
};

#endif