#ifndef SCENE_ROCK_H
#define SCENE_ROCK_H

#include "../Scene.h"
#include "../../GameObject/ItemProjectiles.h"

class SceneRock : public Scene
{
    //float m_worldHeight;
    //float m_worldWidth;

    // Clicking
    bool bLButtonState;

public:
    SceneRock();
    ~SceneRock();

    virtual void Init();
    virtual void Update(double dt);
    virtual void Render();
    virtual void RenderRockScene();
    virtual void Exit();
    bool ViewCheckPosition(Vector3 pos, float degree);
    //Test stuff
    ItemProjectile* itemProjectile;

    //For placing traps
    //bool b_placing = false;
    bool b_capturing = false;
    bool b_captured = false;
    int captureCounter = 0;

    std::vector<Vector3> placedTraps;

    //AABB HITBOX;

    World rockWorld;

    AABB HITBOX;
    double counter;
    GameObject monster, rock, net, bait;
};

#endif