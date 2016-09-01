/****************************************************************************/
/*!
\file SceneManager.h
\author Bek Kunta
\par email: xiaobek\@gmail.com
\brief
A Class that handles the different scenes
*/
/****************************************************************************/
#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

//#include "../Scene/Scene.h"
#include "../Scene/mainMenu.h"
#include "../Scene/Pause.h"
#include "../Scene/Faint.h"

/****************************************************************************/
/*!
Class SceneManager:
\brief Defines a class that manages the different scenes in gameplay
*/
/****************************************************************************/
class SceneManager
{
public:
    enum GAME_STATE
    {
        GAMESTATE_MAINMENU,
        GAMESTATE_GAMEPLAY,
        GAMESTATE_PAUSE,
        GAMESTATE_EXIT,
		GAMESTATE_FAINT,

        GAMESTATE_TOTAL
    };

private:
    GAME_STATE m_gamestate;

public:
	SceneManager();
	~SceneManager();

	Scene* m_scene;

    MainMenu* m_mainMenu;
    Pause* m_pause;
	Faint* m_faint;

    GAME_STATE GetGameState();

	void ChangeScene(short id);

    void SetMainMenuState();
    void SetGameState();
    void SetPauseState();
	void SetFaintState();
    void SetToExit();

	void Update(double dt);
	void Render();
	void Exit();
};

#endif