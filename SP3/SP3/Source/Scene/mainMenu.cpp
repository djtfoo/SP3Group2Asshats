#include "GL\glew.h"
//#include "../../shader.hpp"

#include "mainMenu.h"
#include "../General/Application.h"
#include "../General/SharedData.h"

#include <sstream>

MainMenu::MainMenu()
{
}

MainMenu::~MainMenu()
{
}

void MainMenu::Init()
{
	MM_STATE = M_MAIN;

	Application::GetCursorPos(&Application::cursorXPos, &Application::cursorYPos);
	Application::SetCursorPos(Application::GetWindowWidth() / 2.f, Application::GetWindowHeight() / 2.f);

	for (int i = 0; i < 7; i++)
	{
		button_highlighted[i] = false;
	}

	b_mouseClick = false;
}

void MainMenu::Update(double dt)
{
	//double x, y;
	//Application::GetCursorPos(&x, &y);

	switch (MM_STATE)
	{
	case M_MAIN:
	{
		MainMenuButton();
	}
	case M_INSTRUCTION:
	{
		BackButton();
	}
	case M_STORY:
	{
		BackButton();
	}
	case M_OPTION:
	{
		BackButton();
	}
	case M_CREDITS:
	{
		BackButton();
	}
	}
}

void MainMenu::Render()
{
	double x, y;
	Application::GetCursorPos(&x, &y);
	//std::stringstream ss;

	//ss.str("");
	//ss << "X: " << x;
	//scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3, 30, 6);

	//ss.str("");
	//ss << "Y: " << y;
	//scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3, 30, 9);

	scene->SetHUD(true);
	switch (MM_STATE)
	{
	case M_MAIN:
		RenderMainMenuState();
		break;
	case M_INSTRUCTION:
		RenderInstructionState();
		break;
	case M_STORY:
		RenderStoryState();
		break;
	case M_OPTION:
		RenderOptionState();
		break;
	case M_CREDITS:
		RenderCreditState();
		break;
	}
	scene->SetHUD(false);
}

void MainMenu::RenderMainMenuState()
{
	// Buttons
	if (button_highlighted[0])
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_STARTGAME_HL), false, 26.f, 6.5f, -55, 50);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_STARTGAME_ICON), false, 26.f, 6.5f, -29, 50);
		if (!b_playOnes[0])
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseHover.wav");
			b_playOnes[0] = true;
		}
	}
	else
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_STARTGAME), false, 24.f, 6.f, -55, 50);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_STARTGAME_ICON), false, 24.f, 6.f, -29, 50);
		b_playOnes[0] = false;
	}

	if (button_highlighted[1])
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_LOADGAME_HL), false, 26.f, 6.5f, -55, 35);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_LOADGAME_ICON), false, 26.f, 6.5f, -29, 35);
		if (!b_playOnes[1])
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseHover.wav");
			b_playOnes[1] = true;
		}
	}
	else
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_LOADGAME), false, 24.f, 6.f, -55, 35);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_LOADGAME_ICON), false, 24.f, 6.f, -29, 35);
		b_playOnes[1] = false;
	}

	if (button_highlighted[2])
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_INSTRUCTIONS_HL), false, 26.f, 6.5f, -55, 20);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_INSTRUCTIONS_ICON), false, 14.f, 4.5f, -34, 20);
		if (!b_playOnes[2])
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseHover.wav");
			b_playOnes[2] = true;
		}
	}
	else
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_INSTRUCTIONS), false, 24.f, 6.f, -55, 20);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_INSTRUCTIONS_ICON), false, 12.f, 4.f, -34, 20);
		b_playOnes[2] = false;
	}

	if (button_highlighted[3])
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_STORY_HL), false, 26.f, 6.5f, -55, 5);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_STORY_ICON), false, 14.f, 4.5f, -34, 5);
		if (!b_playOnes[3])
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseHover.wav");
			b_playOnes[3] = true;
		}
	}
	else
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_STORY), false, 24.f, 6.f, -55, 5);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_STORY_ICON), false, 12.f, 4.f, -34, 5);
		b_playOnes[3] = false;
	}

	if (button_highlighted[4])
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_OPTIONS_HL), false, 26.f, 6.5f, -55, -10);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_OPTIONS_ICON), false, 14.f, 4.5f, -34, -10);
		if (!b_playOnes[4])
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseHover.wav");
			b_playOnes[4] = true;
		}
	}
	else
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_OPTIONS), false, 24.f, 6.f, -55, -10);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_OPTIONS_ICON), false, 12.f, 4.f, -34, -10);
		b_playOnes[4] = false;
	}

	if (button_highlighted[5])
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CREDITS_HL), false, 26.f, 6.5f, -55, -25);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CREDITS_ICON), false, 14.f, 4.5f, -34, -25);
		if (!b_playOnes[5])
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseHover.wav");
			b_playOnes[5] = true;
		}
	}
	else
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CREDITS), false, 24.f, 6.f, -55, -25);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CREDITS_ICON), false, 12.f, 4.f, -34, -25);
		b_playOnes[5] = false;
	}

	if (button_highlighted[6])
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_EXIT_HL), false, 26.f, 6.5f, -55, -40);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_EXIT_ICON), false, 14.f, 4.5f, -34, -40);
		if (!b_playOnes[6])
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseHover.wav");
			b_playOnes[6] = true;
		}
	}
	else
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_EXIT), false, 24.f, 6.f, -55, -40);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_EXIT_ICON), false, 12.f, 4.f, -34, -40);
		b_playOnes[6] = false;
	}

	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GREENBUTTON), false, 30.f, 10.f, -55, 50);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 48.f, 12.f, -50, 50);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GREENBUTTON), false, 30.f, 10.f, -55, 35);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 48.f, 12.f, -50, 35);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GREENBUTTON), false, 30.f, 10.f, -55, 20);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 48.f, 12.f, -50, 20);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GREENBUTTON), false, 30.f, 10.f, -55, 5);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 48.f, 12.f, -50, 5);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GREENBUTTON), false, 30.f, 10.f, -55, -10);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 48.f, 12.f, -50, -10);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GREENBUTTON), false, 30.f, 10.f, -55, -25);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 48.f, 12.f, -50, -25);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GREENBUTTON), false, 30.f, 10.f, -55, -40);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 48.f, 12.f, -50, -40);

	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GAMETITLE), false, 72.f, 18.f, 30, 44);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 84.f, 22.f, 30, 44);

}

void MainMenu::RenderInstructionState()
{
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_INSTRUCTIONS), false, 48.f, 12.f, 0, 45);
	RenderBackButton();
}

void MainMenu::RenderStoryState()
{
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_STORY), false, 48.f, 12.f, 0, 45);
	RenderBackButton();
}

void MainMenu::RenderCreditState()
{
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CREDITS), false, 48.f, 12.f, 0, 45);
	RenderBackButton();
}

void MainMenu::RenderOptionState()
{
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_OPTIONS), false, 48.f, 12.f, 0, 45);
	RenderBackButton();
}

/////////////////////////
/////BUTTON STUFFS///////
/////////////////////////

void MainMenu::MainMenuButton()
{
	double x, y;
	Application::GetCursorPos(&x, &y);

	if (x > 192 && x < 525 && y > 53 && y < 123)
	{
		button_highlighted[0] = true;
	}
	else
	{
		button_highlighted[0] = false;
	}

	if (button_highlighted[0])
	{
		if (!b_mouseClick && Application::IsMousePressed(0))
		{
			b_mouseClick = true;
		}
		else if (b_mouseClick && !Application::IsMousePressed(0))
		{
			b_mouseClick = false;
			//SharedData::GetInstance()->sound->playSoundEffect("Sound//Click.mp3");
			std::cout << "STARTGAME" << std::endl;
			SharedData::GetInstance()->sceneManager->SetGameState();
            SharedData::GetInstance()->sceneManager->ChangeScene(5);
		}
	}

	if (x > 192 && x < 525 && y > 183 && y < 253)
	{
		button_highlighted[1] = true;
	}
	else
	{
		button_highlighted[1] = false;
	}

	if (button_highlighted[1])
	{
        // Load game
        if (!b_mouseClick && Application::IsMousePressed(0))
        {
            b_mouseClick = true;
        }
        else if (b_mouseClick && !Application::IsMousePressed(0))
        {
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseClick.wav");
			b_mouseClick = false;
			//LoadStuff Here5
		}
	}

	if (x > 192 && x < 525 && y > 316 && y < 386)
	{
		button_highlighted[2] = true;
	}
	else
	{
		button_highlighted[2] = false;
	}

	if (button_highlighted[2])
	{
		if (!b_mouseClick && Application::IsMousePressed(0))
		{
			b_mouseClick = true;
		}
		else if (b_mouseClick && !Application::IsMousePressed(0))
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseClick.wav");
			b_mouseClick = false;
			MM_STATE = M_INSTRUCTION;
		}
	}

	if (x > 192 && x < 525 && y > 450 && y < 520)
	{
		button_highlighted[3] = true;
	}
	else
	{
		button_highlighted[3] = false;
	}

	if (button_highlighted[3])
	{
		if (!b_mouseClick && Application::IsMousePressed(0))
		{
			b_mouseClick = true;
		}
		else if (b_mouseClick && !Application::IsMousePressed(0))
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseClick.wav");
			b_mouseClick = false;
			MM_STATE = M_STORY;
		}
	}

	if (x > 192 && x < 525 && y > 580 && y < 650)
	{
		button_highlighted[4] = true;
	}
	else
	{
		button_highlighted[4] = false;
	}

	if (button_highlighted[4])
	{
		if (!b_mouseClick && Application::IsMousePressed(0))
		{
			b_mouseClick = true;
		}
		else if (b_mouseClick && !Application::IsMousePressed(0))
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseClick.wav");
			b_mouseClick = false;
			MM_STATE = M_OPTION;
		}
	}

	if (x > 192 && x < 525 && y > 710 && y < 780)
	{
		button_highlighted[5] = true;
	}
	else
	{
		button_highlighted[5] = false;
	}

	if (button_highlighted[5])
	{
		if (!b_mouseClick && Application::IsMousePressed(0))
		{
			b_mouseClick = true;
		}
		else if (b_mouseClick && !Application::IsMousePressed(0))
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseClick.wav");
			b_mouseClick = false;
			MM_STATE = M_CREDITS;
		}
	}
	if (x > 192 && x < 525 && y > 845 && y < 915)
	{
		button_highlighted[6] = true;
	}
	else
	{
		button_highlighted[6] = false;
	}

	if (button_highlighted[6])
	{
		if (!b_mouseClick && Application::IsMousePressed(0))
		{
			b_mouseClick = true;
		}
		else if (b_mouseClick && !Application::IsMousePressed(0))
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseClick.wav");
			b_mouseClick = false;
			std::cout << "EXIT" << std::endl;
			SharedData::GetInstance()->sceneManager->SetToExit();
		}
	}
}

/////////////////////////
//RENDER BUTTON STUFFS///
/////////////////////////

void MainMenu::RenderBackButton()
{
	if (button_highlighted[7])
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACK_HL), false, 26.f, 6.5f, 45, -45);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACK_ICON), false, 14.f, 4.5f, 66, -45);
		if (!b_playOnes[7])
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseHover.wav");
			b_playOnes[7] = true;
		}
	}
	else
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACK), false, 24.f, 6.f, 45, -45);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACK_ICON), false, 12.f, 4.f, 66, -45);
		b_playOnes[7] = false;
	}
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GREENBUTTON), false, 30.f, 10.f, 45, -45);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 48.f, 12.f, 50, -45);
}

void MainMenu::BackButton()
{
	bool b_StartBtnHL;
	bool b_BackBtnHL;
	double x, y;
	Application::GetCursorPos(&x, &y);
	if (x > 1390 && x < 1725 && y > 888 && y < 960)
	{
		button_highlighted[7] = true;
	}
	else
	{
		button_highlighted[7] = false;
	}

	if (button_highlighted[7])
	{
		if (!b_mouseClick && Application::IsMousePressed(0))
		{
			b_mouseClick = true;
		}
		else if (b_mouseClick && !Application::IsMousePressed(0))
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseClick.wav");
			b_mouseClick = false;
			MM_STATE = M_MAIN;
		}
	}
}

void MainMenu::Exit()
{

}