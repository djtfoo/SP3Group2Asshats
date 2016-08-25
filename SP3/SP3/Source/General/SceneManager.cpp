/****************************************************************************/
/*!
\file SceneManager.cpp
\author Bek Kunta
\par email: xiaobek\@gmail.com
\brief
A Class that handles the different scenes
*/
/****************************************************************************/
#include "SceneManager.h"
//#include "../Scene/MainMenu/MenuScene.h"
#include "../Scene/Zone 1/SceneSP3.h"

#include "../Scene/Zone 1/SceneGrass.h"
#include "../Scene/Zone 2/SceneSwamp.h"
#include "../Scene/Zone 3/SceneRock.h"
#include "../Scene/Zone 4/SceneLava.h"
#include "../Scene/Zoo/SceneZoo.h"

SceneManager::SceneManager() : m_scene(0)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::ChangeScene(short id)
{
	if (m_scene)
	{
		m_scene->Exit();
		delete m_scene;
	}

	if (id == 0)
	{
		//m_scene = new MenuScene();
	}
	else if (id == 1)
	{
        m_scene = new SceneGrass();
	}
	else if (id == 2)
	{
		m_scene = new SceneSwamp();
	}
    else if (id == 3)
    {
        m_scene = new SceneRock();
    }
    else if (id == 4)
    {
        m_scene = new SceneLava();
    }
    else if (id == 5)
    {
        m_scene = new SceneZoo();
    }

	if (m_scene)
	{
		m_scene->Init();
	}
}

void SceneManager::Update(double dt)
{
	if (m_scene)
	{
		m_scene->Update(dt);
	}
}

void SceneManager::Render()
{
	if (m_scene)
	{
		m_scene->Render();
	}
}

void SceneManager::Exit()
{
	if (m_scene)
	{
		m_scene->Exit();
		delete m_scene;
	}
}