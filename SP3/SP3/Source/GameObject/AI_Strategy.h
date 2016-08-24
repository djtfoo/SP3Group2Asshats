/******************************************************************************/
/*!
\file	AI_Strategy.h
\author Muhammad Amirul Bin Zaol-kefli
\par	email: 150509T@mymail.nyp.edu.sg
\brief
Class that Updates the strategy of monster
*/
/******************************************************************************/
#ifndef AI_STRATEGY_H
#define AI_STRATEGY_H

#include "Vector3.h"
//#include "Player.h"
#include "Monster.h"
#include "AABB.h"

class AI_Strategy
{
public:
	AI_Strategy();
	~AI_Strategy();

	enum STRATEGY_MODE
	{
		STATE_IDLE,
		STATE_ATTACK,
		STATE_RUN,
		STATE_TRAPPED,	//stun
		TOTAL_AI_STATE,
	};

	STRATEGY_MODE GetCurrentStrategy();
	void SetState(STRATEGY_MODE currentState);
	STRATEGY_MODE GetState();

    void Init(Monster* monster);
	void Update();
	//virtual void SetDestination(Vector3 Destination) = 0;
	//virtual Vector3 GetPosition(Vector3 monsterPos) = 0;

	int CalculateDistance(const Vector3& MonsterPos, const Vector3& Destination);

	//Vector3 Destination;

private:
	Monster* monster;

	STRATEGY_MODE currentState;
	//AABB hitbox;
	//Vector3 destination;

	//Player* player;
	//Monster* monster;
	//std::vector<Vector3> placedTraps;
	//ItemProjectile* bait;
};

#endif