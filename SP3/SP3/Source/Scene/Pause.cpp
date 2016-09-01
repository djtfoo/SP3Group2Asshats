#include "GL\glew.h"
//#include "../../shader.hpp"

#include "Pause.h"
#include "../General/Application.h"
#include "../General/SharedData.h"

#include <sstream>

Pause::Pause()
{
}

Pause::~Pause()
{
}

void Pause::Init()
{
	P_STATE = P_MAIN;

	Application::GetCursorPos(&Application::cursorXPos, &Application::cursorYPos);
	Application::SetCursorPos(Application::GetWindowWidth() / 2.f, Application::GetWindowHeight() / 2.f);

	for (int i = 0; i < 7; i++)
	{
		button_highlighted[i] = false;
	}

	b_mouseClick = false;
}

void Pause::Update(double dt)
{
	switch (P_STATE)
	{
	case P_MAIN:
	{
		PauseMenuButton();
	}
	case P_QUEST:
	{
		BackButton();
	}
	case P_OPTION:
	{
		BackButton();
	}
	}
}

void Pause::Render()
{
	double x, y;
	Application::GetCursorPos(&x, &y);

	scene->SetHUD(true);
	switch (P_STATE)
	{
	case P_MAIN:
		RenderPauseMenuState();
		break;
	case P_QUEST:
		RenderQuestState();
		break;
	case P_OPTION:
		RenderOptionState();
		break;
	}
	scene->SetHUD(false);
}

void Pause::RenderPauseMenuState()
{
	std::stringstream ss;

	if (button_highlighted[0])
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_RESUMEGAME_HL), false, 26.f, 6.5f, -55, 50);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_RESUMEGAME_ICON), false, 14.f, 4.5f, -34, 50);
		if (!b_playOnes[0])
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseHover.wav");
			b_playOnes[0] = true;
		}
	}
	else
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_RESUMEGAME), false, 24.f, 6.f, -55, 50);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_RESUMEGAME_ICON), false, 12, 4.f, -34, 50);
		b_playOnes[0] = false;
	}

	if (button_highlighted[1])
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACKTOTOWN_HL), false, 26.f, 6.5f, -55, 35);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACKTOTOWN_ICON), false, 14.f, 4.5f, -34, 35);
		if (!b_playOnes[1])
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseHover.wav");
			b_playOnes[1] = true;
		}
	}
	else
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACKTOTOWN), false, 24.f, 6.f, -55, 35);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACKTOTOWN_ICON), false, 12, 4.f, -34, 35);
		b_playOnes[1] = false;
	}

	if (button_highlighted[2])
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_QUEST_HL), false, 26.f, 6.5f, -55, 20);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_QUEST_ICON), false, 14.f, 4.5f, -34, 20);
		if (!b_playOnes[2])
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseHover.wav");
			b_playOnes[2] = true;
		}
	}
	else
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_QUEST), false, 24.f, 6.f, -55, 20);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_QUEST_ICON), false, 12, 4.f, -34, 20);
		b_playOnes[2] = false;
	}

	if (button_highlighted[3])
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_OPTIONS_HL), false, 26.f, 6.5f, -55, 5);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_OPTIONS_ICON), false, 14.f, 4.5f, -34, 5);
		if (!b_playOnes[3])
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseHover.wav");
			b_playOnes[3] = true;
		}
	}
	else
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_OPTIONS), false, 24.f, 6.f, -55, 5);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_OPTIONS_ICON), false, 12, 4.f, -34, 5);

		b_playOnes[3] = false;
	}

	if (button_highlighted[4])
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_EXIT_HL), false, 26.f, 6.5f, -55, -10);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_EXIT_ICON), false, 14.f, 4.5f, -34, -10);
		if (!b_playOnes[4])
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseHover.wav");
			b_playOnes[4] = true;
		}
	}
	else
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_EXIT), false, 24.f, 6.f, -55, -10);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_EXIT_ICON), false, 12, 4.f, -34, -10);
		b_playOnes[4] = false;
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


	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_PAUSEGAME), false, 48.f, 12.f, 20, 48);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 60.f, 16.f, 20, 48);
	
}

void Pause::RenderQuestState()
{
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_QUEST_HL), false, 48.f, 12.f, 0, 45);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 60.f, 15.f, 0, 45);

	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 140.f, 70.f, 0, 0);

	std::stringstream ss;
	ss << SharedData::GetInstance()->questManager->GetCurrentQuest()->GetQuestName();
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1,1), 6, 20, 37);

	ss.str("");
	ss << SharedData::GetInstance()->questManager->GetCurrentQuest()->GetSerialNumber() << ". ";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 1), 6, 15, 37);

	ss.str("");
	ss << SharedData::GetInstance()->questManager->GetCurrentQuest()->GetRequiredMonster() << ":  ";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 1), 3, 15,  30);

	ss.str("");
	ss << "0"<<" / " <<SharedData::GetInstance()->questManager->GetCurrentQuest()->GetRequiredQuantity() ;
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 1), 3, 25, 30);

	ss.str("");
	ss << "Location: " <<SharedData::GetInstance()->questManager->GetCurrentQuest()->GetZone();
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 1), 3, 15, 20);

	RenderBackButton();
}

void Pause::RenderOptionState()
{
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_OPTIONS_HL), false, 48.f, 12.f, 0, 45);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 60.f, 15.f, 0, 45);
	RenderBackButton();
}

/////////////////////////
/////BUTTON STUFFS///////
/////////////////////////

void Pause::PauseMenuButton()
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
			std::cout << "RESUME GAME" << std::endl;
			SharedData::GetInstance()->sceneManager->SetGameState();
            Application::SetDisabledCursor();
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
		if (!b_mouseClick && Application::IsMousePressed(0))
		{
			b_mouseClick = true;
		}
		else if (b_mouseClick && !Application::IsMousePressed(0))
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseClick.wav");
			b_mouseClick = false;
			std::cout << "BACK TO TOWN" << std::endl;
            SharedData::GetInstance()->sceneManager->SetGameState();
            SharedData::GetInstance()->sceneManager->ChangeScene(5);
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
			P_STATE = P_QUEST;
			//SHOW QUEST;
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
			P_STATE = P_OPTION;
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
            SharedData::GetInstance()->sceneManager->ChangeScene(Math::RandIntMinMax(1, 4));
            SharedData::GetInstance()->sceneManager->SetMainMenuState();
		}
	}
}

/////////////////////////
//RENDER BUTTON STUFFS///
/////////////////////////

void Pause::RenderBackButton()
{
	if (button_highlighted[5])
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACK_HL), false, 26.f, 6.5f, 45, -45);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACK_ICON), false, 14.f, 4.5f, 66, -45);
		if (!b_playOnes[5])
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseHover.wav");
			b_playOnes[5] = true;
		}
	}
	else
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACK), false, 24.f, 6.f, 45, -45);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACK_ICON), false, 12.f, 4.f, 66, -45);
		b_playOnes[5] = false;
	}
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GREENBUTTON), false, 30.f, 10.f, 45, -45);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 48.f, 12.f, 50, -45);
}

void Pause::BackButton()
{
	double x, y;
	Application::GetCursorPos(&x, &y);
	if (x > 1390 && x < 1725 && y > 888 && y < 960)
	{
		button_highlighted[5] = true;
	}
	else
	{
		button_highlighted[5] = false;
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
			P_STATE = P_MAIN;
		}
	}
}

void Pause::Exit()
{

}