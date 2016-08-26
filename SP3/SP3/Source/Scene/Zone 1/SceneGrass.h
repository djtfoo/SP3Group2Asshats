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
	SceneGrass();
	~SceneGrass();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void RenderGrassScene();
	virtual void RenderHUD();
	virtual void Exit();
    void RenderMonsterStates();
	bool ViewCheckPosition(Vector3 pos, float degree);
    //Test stuff
	ItemProjectile* itemProjectile;
	ItemProjectile* rockProjectile;
	ItemProjectile* netProjectile;
	ItemProjectile* baitProjectile;

    //For placing traps
    bool b_placing = false;

	//Item Switch
	bool b_Rocks;
	bool b_Nets;
	bool b_Baits;

	float f_RotateRock;
	float f_RotateNet;
	float f_RotateBait;

    std::vector<Vector3> placedTraps;

	//AABB HITBOX;

    World grass;

    AABB HITBOX;
    double counter;
    GameObject monster, rock, net, bait,text_C,text_M;
};

#endif