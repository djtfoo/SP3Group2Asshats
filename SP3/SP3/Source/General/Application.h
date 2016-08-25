#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"
#include "../Scene/Scene.h"
#include "SceneManager.h"
#include "InputManager.h"

class Application
{
public:
	static Application& GetInstance()
	{
		static Application app;
		return app;
	}
	void Init();
	void Run();
	void Exit();
	static bool IsKeyPressed(unsigned short key);
	static bool IsMousePressed(unsigned short key);
	static void GetCursorPos(double *xpos, double *ypos);
	static void SetCursorPos(double xpos, double ypos);

    void GetMonitorResolution();

	static int GetWindowWidth();
	static int GetWindowHeight();

    static int m_width;
    static int m_height;

    static double cursorXPos;
    static double cursorYPos;

    static double mouseWheelX;
    static double mouseWheelY;

    // Game Scenes
    SceneManager *sceneManager;

private:
	Application();
	~Application();

	//Declare a window object
	StopWatch m_timer;
};

#endif