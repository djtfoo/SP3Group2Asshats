/******************************************************************************/
/*!
\file	SharedData.h
\author Foo Jing Ting
\par	email: 152856H\@mymail.nyp.edu.sg
\brief
Singleton class that contains variables and objects needed by different files
*/
/******************************************************************************/
#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include "../GameObject/Player.h"
#include "../Graphics/GraphicsLoader/GraphicsLoader.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "../Audio/Sound.h"
#include "../Graphics/Particle/ParticleManager.h"
#include "Quest/QuestManager.h"
#include "SaveData.h"

/******************************************************************************/
/*!
Class SharedData:
\brief	Singleton class containing variables and objects needed by different files
*/
/******************************************************************************/
class SharedData
{
	SharedData();
	~SharedData();

public:
	void Init();
    void Exit();

	static SharedData* GetInstance()
	{
		static SharedData sharedData;
		return &sharedData;
	}

	Player* player;
	GraphicsLoader* graphicsLoader;
    InputManager* inputManager;
	Sound* sound;
	ParticleManager* particleManager;

    SceneManager* sceneManager;
    QuestManager* questManager;

    SaveData* saveData;     //store's last save, in case player wishes to load game
};

#endif