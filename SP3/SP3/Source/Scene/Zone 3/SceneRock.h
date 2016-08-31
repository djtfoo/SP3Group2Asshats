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
    virtual void Exit();
    void RenderRockScene();
    virtual bool CheckInteractMoneyTree(World *world, GameObject GO);

    //Test stuff
    ItemProjectile* itemProjectile;
    ItemProjectile* rockProjectile;
    ItemProjectile* netProjectile;
    ItemProjectile* baitProjectile;

    //For placing traps
    bool b_placing = false;
    //bool b_capturing = false;
    //bool b_captured = false;
    //int captureCounter = 0;

    std::vector<Vector3> placedTraps;

    //AABB HITBOX;

    World rockWorld;

    AABB HITBOX;
    GameObject monster, rock, net, bait;
};

#endif