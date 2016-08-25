#ifndef SCENE_ZOO_H
#define SCENE_ZOO_H

#include "../Scene.h"
#include "../../GameObject/ItemProjectiles.h"
#include "../../General/ZooCamera.h"
#include "../../General/MousePicker.h"

class SceneZoo : public Scene
{
    //float m_worldHeight;
    //float m_worldWidth;

    // Clicking
    bool bLButtonState;

public:
    SceneZoo();
    ~SceneZoo();

    virtual void Init();
    virtual void Update(double dt);
    virtual void Render();
    virtual void RenderZooScene();
    virtual void Exit();

    //For placing traps
    //bool b_placing = false;
    bool b_capturing = false;
    bool b_captured = false;
    int captureCounter = 0;

    //AABB HITBOX;
    World zooWorld;
    ZooCamera zooCamera;
    double counter;

    std::vector<Vector3> tempStore;

};

#endif