#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "Scene.h"

class MainMenu
{
	enum MAINMENU_STATE
	{
		M_MAIN = 0,
		M_PLAY,
		M_LOADGAME,
		M_INSTRUCTION,
		M_STORY,
		M_OPTION,
		M_CREDITS,
		M_INSTRUCTION2,
		M_INSTRUCTION3,
		M_EXIT,

		M_TOTAL
	};
public:
	Scene* scene; // to render on top

	MainMenu();
	~MainMenu();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	bool b_mouseClick;

	float m_worldHeight;
	float m_worldWidth;

	bool b_playOnes[M_TOTAL];

	MAINMENU_STATE MM_STATE;
	bool button_highlighted[M_TOTAL];

	bool b_StartBtnHL;
	bool b_BackBtnHL;

	void RenderMainMenuState();
	void RenderInstructionState();
	void RenderInstructionState2();
	void RenderInstructionState3();
	void RenderStoryState();
	void RenderOptionState();
	void RenderCreditState();

	void MainMenuButton();
	void BackButton();
	void NextButton();
	void NextButtonMonstersPage();

	void RenderNextButton();
	void RenderButtonMonstersPage();
	void RenderBackButton();
};

#endif