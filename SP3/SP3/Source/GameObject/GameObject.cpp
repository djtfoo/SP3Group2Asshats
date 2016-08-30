#include "GameObject.h"

unsigned int createGO(World *world)
{
	for (GameObject GO = 0; GO < world->GAMEOBJECT_COUNT; ++GO)
	{
		if (world->mask[GO] == COMPONENT_NONE)
		{
			//Returns the first int that does not have a componenet/is inactive
			//std::cout << "created go" << std::endl;
			return(GO);
		}
	}

	//Already at Max GOs
	std::cout << "at max go" << std::endl;
	return(world->GAMEOBJECT_COUNT);
}

void destroyGO(World *world, GameObject GO)
{
	if ((world->mask[GO] & COMPONENT_DISPLACEMENT) == COMPONENT_DISPLACEMENT)
		world->position[GO].SetZero();

	if ((world->mask[GO] & COMPONENT_VELOCITY) == COMPONENT_VELOCITY)
		world->velocity[GO].SetZero();

	if ((world->mask[GO] & COMPONENT_APPEARANCE) == COMPONENT_APPEARANCE)
	{
		world->appearance[GO].scale.SetZero();
		world->appearance[GO].mesh = nullptr;
        world->appearance[GO].angle = 0.f;
	}

	if ((world->mask[GO] & COMPONENT_HITBOX) == COMPONENT_HITBOX)
	{
		world->hitbox[GO].m_origin.SetZero();
		world->hitbox[GO].m_scale.SetZero();
	}

	if ((world->mask[GO] & COMPONENT_TRAP) == COMPONENT_TRAP)
	{
		world->trap[GO].radius = 0;
		world->trap[GO].triggerDuration = 0;
		world->trap[GO].triggerTimer = 0;
		world->trap[GO].activated = false;
		world->trap[GO].caughtMonster = 0;
		world->trap[GO].caughtMonsterVel.SetZero();
	}

	if ((world->mask[GO] & COMPONENT_CAPTURE) == COMPONENT_CAPTURE)
	{
		world->capture[GO].capturedMonster = false;
		world->capture[GO].capturingMonster = false;
		world->capture[GO].timeBeforeCapture = 0;
        world->capture[GO].caughtMonster = 0;
	}
	if ((world->mask[GO] & COMPONENT_BAIT) == COMPONENT_BAIT)
	{
		world->bait[GO].eattingBait = false;
		world->bait[GO].foundBait = false;
		world->bait[GO].scentRadius = 0;
		world->bait[GO].foundRadius = 0;
		world->bait[GO].timeEatting = 0;
        world->bait[GO].finishedBait = false;
        world->bait[GO].b_PlayBaitSoundOnes = false;
    }

    if (world->monster[GO])
    {
        delete world->monster[GO];
        world->monster[GO] = 0;
    }


	world->mask[GO] = COMPONENT_NONE;
    std::cout << "destroyed go" << std::endl;
}