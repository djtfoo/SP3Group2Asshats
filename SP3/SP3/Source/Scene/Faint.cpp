#include "GL\glew.h"
//#include "../../shader.hpp"

#include "Faint.h"
#include "../General/Application.h"
#include "../General/SharedData.h"

#include <sstream>

Faint::Faint()
{
}

Faint::~Faint()
{
}

void Faint::Init()
{
	Application::GetCursorPos(&Application::cursorXPos, &Application::cursorYPos);
	Application::SetCursorPos(Application::GetWindowWidth() / 2.f, Application::GetWindowHeight() / 2.f);

	button_highlighted = false;

	b_mouseClick = false;

}

void Faint::Update(double dt)
{
	FaintMenuButton();

	if (!b_DeathSound)
	{
		SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//MouseClick.wav",
			irrklang::vec3df(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z),
			irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
			irrklang::vec3df(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z));
		b_DeathSound = true;
	}
}

void Faint::Render()
{
	double x, y;
	Application::GetCursorPos(&x, &y);

	scene->SetHUD(true);

	RenderFaintMenuState();

	scene->SetHUD(false);
}

void Faint::RenderFaintMenuState()
{
	std::stringstream ss;

	if (button_highlighted)
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACKTOTOWN_HL), false, 26.f, 6.5f, -55, 35);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACKTOTOWN_ICON), false, 14.f, 4.5f, -34, 35);
		if (!b_playOnes)
		{
			SharedData::GetInstance()->sound->PlaySoundEffect("Sound//MouseHover.wav");
			b_playOnes = true;
		}
	}
	else
	{
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACKTOTOWN), false, 24.f, 6.f, -55, 35);
		scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACKTOTOWN_ICON), false, 12, 4.f, -34, 35);
		b_playOnes = false;
	}

	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 140.f, 70.f, 0, -10);

	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GREENBUTTON), false, 30.f, 10.f, -55, 35);
	scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MENUBOARD), false, 48.f, 12.f, -50, 35);

	ss.str("");
	ss <<"You have blacked out.";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0, 0, 0), 6, 15, 30);

	ss.str("");
	ss << "The monsters you have caught have been";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 0, 0), 3, 15, 20);

	ss.str("");
	ss << "released back to the wild.";
	scene->RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 0, 0), 3, 15, 17);

}

/////////////////////////
/////BUTTON STUFFS///////
/////////////////////////

void Faint::FaintMenuButton()
{
	double x, y;
	Application::GetCursorPos(&x, &y);


	if (x > 192 && x < 525 && y > 183 && y < 253)
	{
		button_highlighted = true;
	}
	else
	{
		button_highlighted = false;
	}

	if (button_highlighted)
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
			b_DeathSound = false;
		}
	}
}

/////////////////////////
//RENDER BUTTON STUFFS///
/////////////////////////

void Faint::Exit()
{

}