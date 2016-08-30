#ifndef PAUSE_H
#define PAUSE_H

#include "Scene.h"

class Pause
{
    //enum PAUSE_STATE?

public:
    Scene* scene; // to render on top

    Pause();
    ~Pause();

    virtual void Init();
    virtual void Update(double dt);
    virtual void Render();
    virtual void Exit();

    bool b_mouseClick;

    float m_worldHeight;
    float m_worldWidth;

    bool button_highlighted[7];

    bool b_StartBtnHL;
    bool b_BackBtnHL;

    void RenderMainMenuState();
    void RenderInstructionState();
    void RenderStoryState();
    void RenderOptionState();
    void RenderCreditState();

    void MainMenuButton();
    void InstructionButton();
    void OptionButton();
    void BackButton();
    void CreditsButton();
    void ExitButton();

    void RenderStartButton();
    void RenderInstructionButton();
    void RenderOptionButton();
    void RenderBackButton();
    void RenderCreditsButton();
    void RenderExitButton();
};

#endif