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
			NextButton();
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
		case M_INSTRUCTION2:
		{
			BackButton();
			NextButtonMonstersPage();
		}
		case M_INSTRUCTION3:
		{
			BackButton();
		}

	}
}

void MainMenu::Render()
{
	double x, y;
	Application::GetCursorPos(&x, &y);

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
		case M_INSTRUCTION2:
			RenderInstructionState2();
			break;
		case M_INSTRUCTION3:
		{
			RenderInstructionState3();
		}
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
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_INSTRUCTIONS_HL), false, 48.f, 12.f, 0, 45);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 60.f, 15.f, 0, 45);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CONTROLS), false, 110.f, 40.f, -5,-2);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 140.f, 70.f, 0, 0);

	std::stringstream ss;
	ss << "Controls: ";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 6, 35, 40);

	
	
	RenderBackButton();
	RenderNextButton();
}

void MainMenu::RenderInstructionState2()
{
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_INSTRUCTIONS_HL), false, 48.f, 12.f, 0, 45);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 60.f, 15.f, 0, 45);

	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 140.f, 70.f, 0, 0);

	std::stringstream ss;
	ss << "Items: ";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0),6, 35, 40);

	ss.str("");
	ss << " :Rocks will damage & increase the chance of catching monsters ";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.5, 18, 37);
	scene->RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCKS1), 2, 15.f, 38.f, 0, 0, 0, false);

	ss.str("");
	ss << " :Nets are used to capture the monsters.";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.5, 18, 32);
	scene->RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET), 1, 15.f, 32.f, 0, 0, 0, false);

	ss.str("");
	ss << " :Baits are used to lure the monsters closer to you.";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.5, 18, 27);
	scene->RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BAIT), 2, 15.f, 28, 0, 0, 0, false);

	ss.str("");
	ss << " :Traps are used to immobilized the monsters for easier capture.";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.5, 18, 22);
	scene->RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TRAP), 2, 15.f, 23.f, 0, 0, 0, false);

	ss.str("");
	ss << " :Meats are used to create baits.";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.5, 18, 17);
	scene->RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MEAT), 2, 15.f,18.f, 0, 0, 0, false);

	RenderBackButton();
	RenderButtonMonstersPage();
}

void MainMenu::RenderInstructionState3()
{
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_INSTRUCTIONS_HL), false, 48.f, 12.f, 0, 45);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 60.f, 15.f, 0, 45);

	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 140.f, 70.f, 0, 0);

	std::stringstream ss;
	scene->RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BAIT), 2, 15.f, 38.f, 0, 0, 0, false);
	scene->RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTERSTATE_RAMPAGE), 4, 15.f, 34.f, 0, 0, 0, false);
	scene->RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTERSTATE_QUESTION_MARK), 4, 15.f, 30.f, 0, 0, 0, false);
	scene->RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTERSTATE_EXCLAMATION_MARK), 4, 15.f, 26.f, 0, 0, 0, false);
	scene->RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_HP), 1, 15.f, 21.f, 0, 0, 0, false);
	scene->RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CAPTUREBAR), 1, 15.f, 23.f, 0, 0, 0, false);
	scene->RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_RABBIT), 2, 15.f, 15.f, 0, 0, 0, false);

	ss << "Monster Characteristics: ";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0),4, 20, 41);

	ss.str("");
	ss << " :Monster in bait state(Will be lured to bait). ";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.5, 17, 37);

	ss.str("");
	ss << " :Monster in Rampage state(Will be lured to player). ";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.5, 17, 33);

	ss.str("");
	ss << " :Monster is alert and senses your presence. ";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.5, 17, 29);

	ss.str("");
	ss << " :Monster will attack or run away from you depending on their behavior. ";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.5, 17, 25);

	ss.str("");
	ss << " :CaptureRate of the monster. ";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.5, 17, 22.5);

	ss.str("");
	ss << " :HP of the monster. ";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.5, 17, 20);


	RenderBackButton();
}

void MainMenu::RenderStoryState()
{
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_STORY_HL), false, 48.f, 12.f, 0, 45);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 60.f, 15.f, 0, 45);

	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 140.f, 70.f, 0, 0);

	std::stringstream ss;

	ss.str("");
	ss << "Ever since Grandad died 5 years ago, the Mon'Colle zoo has been empty.";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.8,10, 40);

	ss.str("");
	ss << "With no one to cull and keep the monsters under control,";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.8, 10, 37.5);

	ss.str("");
	ss << "There has been an influx of monsters appearing near the city.";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.8, 10, 35);

	ss.str("");
	ss << "As the only grandchild left in the family line that is fit to go hunting,";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.8, 10, 32.5);

	ss.str("");
	ss << "It's up to you to capture the monsters and keep the town safe ";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.8, 10, 27.5);

	ss.str("");
	ss << "from the havoc that will reign if the monsters get out of control!  ";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.8, 10, 25);

	ss.str("");
	ss << "Trust your instincts as you dive into the trecherous areas ";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.8, 10, 22.5);

	ss.str("");
	ss << "against the inhospitable climates. ";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.8, 10, 20);

	ss.str("");
	ss << "Manage your coins to upgrade your items and capture all the monsters!";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 1.8, 10, 17.5);

	RenderBackButton();
}

void MainMenu::RenderCreditState()
{
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CREDITS_HL), false, 48.f, 12.f, 0, 45);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 60.f, 15.f, 0, 45);

	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 140.f, 70.f, 0, 0);

	std::stringstream ss;

	ss.str("");
	ss << "SP GROUP 2";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 3, 20, 40);

	ss.str("");
	ss << "(LEADER):   JING TING";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 3, 20, 35);

	ss.str("");
	ss << "(MEMBER): AMIRUL";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 3, 20, 30);

	ss.str("");
	ss << "(MEMBER): KUNTA";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 3, 20, 25);

	ss.str("");
	ss << "(MEMBER): RANDALL";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 3, 20, 20);

	RenderBackButton();
}

void MainMenu::RenderOptionState()
{
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_OPTIONS_HL), false, 48.f, 12.f, 0, 45);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 60.f, 15.f, 0, 45);
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

void MainMenu::RenderNextButton()
{
	if (button_highlighted[8])
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_INSTRUCTIONS_ICON), false, 14.f, 4.5f, -50, -45);
		if (!b_playOnes[8])
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseHover.wav");
			b_playOnes[8] = true;
		}
	}
	else
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_INSTRUCTIONS_ICON), false, 12.f, 4.f, -50, -45);
		b_playOnes[8] = false;
	}
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 48.f, 12.f, -50, -45);

	//scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_EXIT), false, 24.f, 6.f, -55, -40);
	//scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_EXIT_ICON), false, 12.f, 4.f, -34, -40);
	//scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GREENBUTTON), false, 30.f, 10.f, -55, -40);
	//scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 48.f, 12.f, -50, -40);

}

void MainMenu::RenderButtonMonstersPage()
{
	if (button_highlighted[9])
	{
		scene->RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_RABBIT), 2, 15, 6, 0, 0, 0, false);
		//scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_RABBIT), false, 14.f, 4.5f, -50, -45);
		if (!b_playOnes[9])
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseHover.wav");
			b_playOnes[9] = true;
		}
	}
	else
	{
		scene->RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_RABBIT), 1, 15, 6, 0, 0, 0, false);
		b_playOnes[9] = false;
	}
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 48.f, 12.f, -50, -45);

	//scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_EXIT), false, 24.f, 6.f, -55, -40);
	//scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_EXIT_ICON), false, 12.f, 4.f, -34, -40);
	//scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GREENBUTTON), false, 30.f, 10.f, -55, -40);
	//scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 48.f, 12.f, -50, -40);

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

void MainMenu::NextButton()
{
	bool b_StartBtnHL;
	bool b_BackBtnHL;
	double x, y;
	Application::GetCursorPos(&x, &y);

	if (x > 192 && x < 525 && y > 888 && y < 960)
	{
		button_highlighted[8] = true;
	}
	else
	{
		button_highlighted[8] = false;
	}

	if (button_highlighted[8])
	{
		if (!b_mouseClick && Application::IsMousePressed(0))
		{
			b_mouseClick = true;
		}
		else if (b_mouseClick && !Application::IsMousePressed(0))
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseClick.wav");
			b_mouseClick = false;
			MM_STATE = M_INSTRUCTION2;
		}
	}
}

void MainMenu::NextButtonMonstersPage()
{
	bool b_StartBtnHL;
	bool b_BackBtnHL;
	double x, y;
	Application::GetCursorPos(&x, &y);

	if (x > 192 && x < 525 && y > 888 && y < 960)
	{
		button_highlighted[9] = true;
	}
	else
	{
		button_highlighted[9] = false;
	}

	if (button_highlighted[9])
	{
		if (!b_mouseClick && Application::IsMousePressed(0))
		{
			b_mouseClick = true;
		}
		else if (b_mouseClick && !Application::IsMousePressed(0))
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseClick.wav");
			b_mouseClick = false;
			MM_STATE = M_INSTRUCTION3;
		}
	}
}

void MainMenu::Exit()
{

}