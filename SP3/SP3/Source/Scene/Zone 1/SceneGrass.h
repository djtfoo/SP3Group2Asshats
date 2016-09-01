#ifndef SCENE_GRASS_H
#define SCENE_GRASS_H

#include "../Scene.h"
#include "../../GameObject/ItemProjectiles.h"

class SceneGrass : public Scene
{
	//float m_worldHeight;
	//float m_worldWidth;

	// Clicking
	bool bLButtonState;

public:
    SceneGrass(std::string name);
	~SceneGrass();

	virtual void Init();
	virtual void Update(double dt);
    virtual void Render();
    virtual void Exit();
	void RenderGrassScene();
	virtual void SpawnSceneParticles();
    virtual void SceneEnvironmentEffect();

    //Test stuff
	ItemProjectile* itemProjectile;
	ItemProjectile* rockProjectile;
	ItemProjectile* netProjectile;
	ItemProjectile* baitProjectile;

    //For placing traps
    bool b_placing = false;

    std::vector<Vector3> placedTraps;

	//AABB HITBOX;

    World grass;

    AABB HITBOX;
    GameObject monster, rock, net, bait,text_C,text_M;
};

#endif