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
	float f_ParticleSpawnTimer;

public:
    SceneRock(std::string name);
    ~SceneRock();

    virtual void Init();
    virtual void Update(double dt);
    virtual void Render();
    virtual void Exit();
    void RenderRockScene();
    virtual bool CheckInteractMoneyTree(World *world, GameObject GO);
	virtual void SpawnSceneParticles();
    virtual void SceneEnvironmentEffect();

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

    double d_spawnRocksTimer;
    double d_spawnRocksCooldown;
    void UpdateFallingRocks(double dt);

    std::vector<Vector3> placedTraps;

    //AABB HITBOX;

    World rockWorld;

    AABB HITBOX;
    GameObject monster, rock, net, bait;
};

#endif