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

	std::vector<ParticleObject*> particleList;	// Store particles here
	int m_particleCount;	// num of particles
	const unsigned MAX_PARTICLE = 1000;	// max num of particles
	
public:
	SceneGrass();
	~SceneGrass();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void RenderGrassScene();
	virtual void RenderHUD();
	virtual void Exit();
    void RenderPressEText();
	bool ViewCheckPosition(Vector3 pos, float degree);
	void UpdateParticle(double dt);
	ParticleObject* GetParticle(void);
	void RenderParticle(ParticleObject* particle);

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
	
	bool b_PlayBaitSoundOnes;	// Sound for bait

    std::vector<Vector3> placedTraps;

	//AABB HITBOX;

    World grass;

    AABB HITBOX;
    GameObject monster, rock, net, bait,text_C,text_M;
};

#endif