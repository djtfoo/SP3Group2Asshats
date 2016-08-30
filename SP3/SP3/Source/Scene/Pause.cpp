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
    double x, y;
    Application::GetCursorPos(&x, &y);
}

void Pause::Render()
{
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Mtx44 perspective;
    //perspective.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
    ////perspective.SetToOrtho(-80, 80, -60, 60, -1000, 1000);
    //scene->projectionStack.LoadMatrix(perspective);
    //
    //m_worldHeight = 100.f;
    //m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
    //
    //// Camera matrix
    //scene->viewStack.LoadIdentity();
    //scene->viewStack.LookAt(
    //    scene->camera.position.x, scene->camera.position.y, scene->camera.position.z,
    //    scene->camera.target.x, scene->camera.target.y, scene->camera.target.z,
    //    scene->camera.up.x, scene->camera.up.y, scene->camera.up.z
    //	);
    //scene->modelStack.LoadIdentity();

    //double x, y;
    //Application::GetCursorPos(&x, &y);
    //std::stringstream ss;

    //ss.str("");
    //ss << "X: " << x << "   m_worldWidth " << m_worldWidth << "button_highlighted[5] " << button_highlighted[5];
    //RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3, 0, 6);

    //ss.str("");
    //ss << "Y: " << y << "   m_worldHeight " << m_worldHeight << "b_mouseClick " << b_mouseClick;
    //RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3, 0, 9);

    scene->SetHUD(true);
    //switch (MM_STATE)
    //{
    //case M_MAIN:
    //    RenderMainMenuState();
    //    break;
    //case M_INSTRUCTION:
    //    RenderInstructionState();
    //    break;
    //case M_STORY:
    //    RenderStoryState();
    //    break;
    //case M_OPTION:
    //    RenderOptionState();
    //    break;
    //case M_CREDITS:
    //    RenderCreditState();
    //    break;
    //}
    scene->SetHUD(false);
}

void Pause::RenderMainMenuState()
{
    // Buttons
    if (button_highlighted[0])
    {
        scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_START_HL), false, 30.f, 10.f, -60, 35);
    }
    else
    {
        scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_START), false, 24.f, 6.f, -60, 35);
    }

    if (button_highlighted[1])
    {
        scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_INSTRUCTIONS_HL), false, 30.f, 10.f, -60, 25);
    }
    else
    {
        scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_INSTRUCTIONS), false, 24.f, 6.f, -60, 25);
    }

    if (button_highlighted[2])
    {
        scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_STORY_HL), false, 30.f, 10.f, -60, 15);
    }
    else
    {
        scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_STORY), false, 24.f, 6.f, -60, 15);
    }

    if (button_highlighted[3])
    {
        scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_OPTIONS_HL), false, 30.f, 10.f, -60, 5);
    }
    else
    {
        scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_OPTIONS), false, 24.f, 6.f, -60, 5);
    }

    if (button_highlighted[4])
    {
        scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CREDITS_HL), false, 30.f, 10.f, -60, -5);
    }
    else
    {
        scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CREDITS), false, 24.f, 6.f, -60, -5);
    }

    if (button_highlighted[5])
    {
        scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_EXIT_HL), false, 30.f, 10.f, -60, -15);
    }
    else
    {
        scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_EXIT), false, 24.f, 6.f, -60, -15);
    }
}

void Pause::RenderInstructionState()
{
    scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_INSTRUCTIONS), false, 48.f, 12.f, 0, 45);
    RenderBackButton();
}

void Pause::RenderStoryState()
{
    scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_STORY), false, 48.f, 12.f, 0, 45);
    RenderBackButton();
}

void Pause::RenderCreditState()
{
    scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CREDITS), false, 48.f, 12.f, 0, 45);
    RenderBackButton();
}

void Pause::RenderOptionState()
{
    scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_OPTIONS), false, 48.f, 12.f, 0, 45);
    RenderBackButton();
}

/////////////////////////
/////BUTTON STUFFS///////
/////////////////////////

void Pause::MainMenuButton()
{
    double x, y;
    Application::GetCursorPos(&x, &y);

    if (x > 105 && x < 370 && y > 200 && y < 240)
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
        }
    }

    if (x > 105 && x < 370 && y > 290 && y < 330)
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
            SharedData::GetInstance()->sound->playSoundEffect("Sound//NPC.wav");
            b_mouseClick = false;
            //MM_STATE = M_INSTRUCTION;
        }
    }

    if (x > 105 && x < 370 && y > 370 && y < 420)
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
            SharedData::GetInstance()->sound->playSoundEffect("Sound//NPC.wav");
            b_mouseClick = false;
            //MM_STATE = M_STORY;
        }
    }

    if (x > 105 && x < 370 && y > 460 && y < 500)
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
            SharedData::GetInstance()->sound->playSoundEffect("Sound//NPC.wav");
            b_mouseClick = false;
            //MM_STATE = M_OPTION;
        }
    }

    if (x > 105 && x < 370 && y > 550 && y < 590)
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
            SharedData::GetInstance()->sound->playSoundEffect("Sound//NPC.wav");
            b_mouseClick = false;
            //MM_STATE = M_CREDITS;
        }
    }

    if (x > 105 && x < 370 && y > 640 && y < 680)
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
            SharedData::GetInstance()->sound->playSoundEffect("Sound//NPC.wav");
            b_mouseClick = false;
            std::cout << "EXIT" << std::endl;
            SharedData::GetInstance()->sceneManager->SetToExit();
        }
    }
}

/////////////////////////
//RENDER BUTTON STUFFS///
/////////////////////////

void Pause::RenderBackButton()
{
    if (button_highlighted[6])
    {
        scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACK_HL), false, 30.f, 10.f, 60, -45);
    }
    else
    {
        scene->RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BACK), false, 24.f, 6.f, 60, -45);
    }
}

void Pause::BackButton()
{
    bool b_StartBtnHL;
    bool b_BackBtnHL;
    double x, y;
    Application::GetCursorPos(&x, &y);
    if (x > 1600 && x < 1870 && y > 900 && y < 940)
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
            SharedData::GetInstance()->sound->playSoundEffect("Sound//NPC.wav");
            b_mouseClick = false;
            //MM_STATE = M_MAIN;
        }
    }
}

void Pause::Exit()
{

}