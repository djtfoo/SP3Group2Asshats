#ifndef PAUSE_H
#define PAUSE_H

#include "Scene.h"

class Pause
{
	enum PAUSE_STATE
	{
		P_MAIN = 0,
		P_RESUMEGAME,
		P_BACKTOTOWN,
		P_QUEST,
		P_OPTION,
		P_EXITGAME,

		P_TOTAL
	};

public:
	Scene* scene; // to render on top

	Pause();
	~Pause();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	bool b_mouseClick;

	float m_worldHeight;
	float m_worldWidth;

	bool b_playOnes[P_TOTAL];

	PAUSE_STATE P_STATE;
	bool button_highlighted[P_TOTAL];

	bool b_StartBtnHL;
	bool b_BackBtnHL;

	void RenderPauseMenuState();
	void RenderQuestState();
	void RenderOptionState();

	void PauseMenuButton();
	void BackButton();

	void RenderBackButton();
};

#endif