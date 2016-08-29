#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "Scene.h"

using std::string;

class mainMenu : public Scene
{
	enum MAINMENU_STATE
	{
		M_MAIN = 0,
		M_PLAY,
		M_INSTRUCTION,
		M_STORY,
		M_OPTION,
		M_CREDITS,
		M_EXIT,

		M_TOTAL
	};
	public:
		mainMenu();
		~mainMenu();

		virtual void Init();
		virtual void Update(double dt);
		virtual void Render();
		virtual void Exit();

		bool b_mouseClick;

		float m_worldHeight;
		float m_worldWidth;

		MAINMENU_STATE MM_STATE;
		bool button_highlighted[7];

		bool b_StartBtnHL;
		bool b_BackBtnHL;

		void RenderMainMenuState();
		void RenderInstructionState();
		void RenderStoryState();
		void RenderOptionState();
		void RenderCreditState();

		void MainMenuButton();
		void InstructionButton();
		void OptionButton();
		void BackButton();
		void CreditsButton();
		void ExitButton();

		void RenderStartButton();
		void RenderInstructionButton();
		void RenderOptionButton();
		void RenderBackButton();
		void RenderCreditsButton();
		void RenderExitButton();
};

#endif