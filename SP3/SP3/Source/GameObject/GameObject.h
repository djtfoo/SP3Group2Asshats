#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include "Vector3.h"
#include "../Graphics/Mesh/Mesh.h"
#include "AABB.h"
#include "Monster.h"
#include <iostream>

//GO is just an int with reference to the World componenets
typedef unsigned int GameObject;

enum COMPONENTS
{
    COMPONENT_NONE = 0,
    COMPONENT_DISPLACEMENT = 1 << 0,
    COMPONENT_VELOCITY = 1 << 1,
    COMPONENT_APPEARANCE = 1 << 2,
    COMPONENT_HITBOX = 1 << 3,
    COMPONENT_TRAP = 1 << 4,
    COMPONENT_AI = 1 << 5,
    COMPONENT_CAPTURE = 1 << 6,
    COMPONENT_BAIT = 1 << 7,
    COMPONENT_MONEYTREE = 1 << 8,
    COMPONENT_TEXT = 1 << 9,
    COMPONENT_OBSTACLE = 1 << 10,
    COMPONENT_COIN = 1 << 11,
    COMPONENT_MEAT = 1 << 12,
    COMPONENT_ROCKS = 1 << 13,
    COMPONENT_BOULDER = 1 << 14,
    //COMPONENT_TOTAL
};

//Components
struct Appearance
{
    Vector3 scale;
    Mesh* mesh = nullptr;
	bool billboard = false;
    float angle;
};

struct AI_Script
{
    //Strategy start
    //Other stuff
};

struct Health
{
    int maxHealth;
    int currentHealth;
    int regenerationRate;
};

struct Trap_Script
{
    float radius;
    double triggerDuration;
    double triggerTimer;
    bool activated;
    GameObject caughtMonster;
    Vector3 caughtMonsterVel;
};

struct Projectile_Script
{
    float speed;
};

struct Capture_Script
{
	bool capturingMonster;
	bool capturedMonster;
	float timeBeforeCapture;

	float textTimer;
	float textBool;

    GameObject caughtMonster;
};

struct Bait_Script
{
	Vector3 pos;
	bool eattingBait;
	bool foundBait;
	float scentRadius;
	float foundRadius;
	float timeEatting;

    bool b_PlayBaitSoundOnes;

    std::vector<GameObject> baitedMonsters;
    bool finishedBait;
};

struct Money_Tree_Script
{
	Vector3 pos;
	bool destroyed;
	int coins;
};

struct World
{
    static const GameObject GAMEOBJECT_COUNT = 1000;

	GameObject mask[GAMEOBJECT_COUNT];
	Vector3 position[GAMEOBJECT_COUNT];
	Vector3 velocity[GAMEOBJECT_COUNT];
    Appearance appearance[GAMEOBJECT_COUNT];
	AABB hitbox[GAMEOBJECT_COUNT];
    Trap_Script trap[GAMEOBJECT_COUNT];
	Capture_Script capture[GAMEOBJECT_COUNT];
	Bait_Script bait[GAMEOBJECT_COUNT];
    Monster* monster[GAMEOBJECT_COUNT];
};

//"FetchGO"
unsigned int createGO(World *world);
void destroyGO(World *world, unsigned int GO);

//System managers
//void movementFunction(World *world, double dt);
//void renderFunction(World *world);
//ColliderManager;
//ProjectileManager;



#endif