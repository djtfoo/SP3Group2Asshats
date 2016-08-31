#ifndef SCENE_LAVA_H
#define SCENE_LAVA_H

#include "../Scene.h"
#include "../../GameObject/ItemProjectiles.h"
#include "../../Graphics/Lighting/Fog.h"

class SceneLava : public Scene
{
	//float m_worldHeight;
	//float m_worldWidth;
	Fog fog;

	// Clicking
	bool bLButtonState;

public:
    SceneLava(std::string name);
	~SceneLava();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	void RenderLavaScene();
	virtual void Exit();
    virtual bool CheckInteractMoneyTree(World *world, GameObject GO);
	virtual void SpawnSceneParticles();

	//Test stuff
	ItemProjectile* itemProjectile;
	ItemProjectile* rockProjectile;
	ItemProjectile* netProjectile;
	ItemProjectile* baitProjectile;

	//For placing traps
	bool b_placing = false;

	// check if collected
	bool b_RampageMode;

	// monster rampage timer
	float f_RampageTimer;

	std::vector<Vector3> placedTraps;

	//AABB HITBOX;

	World lava;

	AABB HITBOX;
	GameObject monster, rock, net, bait, text_C, text_M;

};

#endif