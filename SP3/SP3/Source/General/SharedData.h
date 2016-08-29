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
#include "../General/InputManager.h"
#include "../Audio/Sound.h"

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
};

#endif