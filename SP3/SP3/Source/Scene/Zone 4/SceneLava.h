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
	SceneLava();
	~SceneLava();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void RenderLavaScene();
	virtual void RenderHUD();
	virtual void Exit();
    void RenderPressEText();
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

    float f_HighlightPos;

	// check if collected
	bool b_Collected;

	// monster rampage timer
	float f_RampageTimer;

	std::vector<Vector3> placedTraps;

	//AABB HITBOX;

	World lava;

	AABB HITBOX;
	double counter;
	GameObject monster, rock, net, bait, text_C, text_M;

};

#endif