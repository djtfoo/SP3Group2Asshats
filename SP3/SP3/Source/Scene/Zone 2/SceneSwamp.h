#ifndef SCENE_SWAMP_H
#define SCENE_SWAMP_H

#include "../Scene.h"
#include "../../GameObject/ItemProjectiles.h"
#include "../../Graphics/Lighting/Fog.h"

class SceneSwamp : public Scene
{
    //float m_worldHeight;
    //float m_worldWidth;

	// Fog
	Fog fog;

    // Clicking
    bool bLButtonState;

public:
    SceneSwamp();
    ~SceneSwamp();

	void UpdateFog(double dt);

    virtual void Init();
    virtual void Update(double dt);
    virtual void Render();
    virtual void RenderSwampScene();
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

    World swamp;

    AABB HITBOX;
    double counter;
    GameObject monster, rock, net, bait;
};

#endif