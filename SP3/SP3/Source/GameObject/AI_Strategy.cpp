/******************************************************************************/
/*!
\file	AI_Strategy.cpp
\author Muhammad Amirul Bin Zaol-kefli
\par	email: 150509T@mymail.nyp.edu.sg
\brief
Class that Updates the strategy of monster
*/
/******************************************************************************/
#include "AI_Strategy.h"
#include "Monster.h"

#include "../Scene/Scene.h"
#include "../General/SharedData.h"

AI_Strategy::AI_Strategy() : currentState(STATE_IDLE)
{
}

AI_Strategy::~AI_Strategy()
{
}

void AI_Strategy::Init(Monster* monster)
{
    this->monster = monster;
}

AI_Strategy::STRATEGY_MODE AI_Strategy::GetState()
{
    return currentState;
}

void AI_Strategy::SetState(AI_Strategy::STRATEGY_MODE currentState)
{
	this->currentState = currentState;

    // change the destination

    switch (this->currentState)
    {
    case STATE_IDLE:
		//monster->ResetAggression();
		//monster->ResetFear();
        SetIdleStateDestination();
        break;
        
    case STATE_ATTACK:
        SetAttackStateDestination();
        break;

    case STATE_RUN:
        SetRunStateDestination();
        break;

	case STATE_RAMPAGE:
		SetRampageStateDestination();
		break;
    }
}

bool AI_Strategy::CheckDestinationReached()
{
    if ((monster->m_position - monster->m_destination).LengthSquared() < (0.5f * Scene::tileSize) * (0.5f * Scene::tileSize))
        return true;

    return false;
}

void AI_Strategy::setDestination(const Vector3& destination)
{
    this->monster->m_destination = destination;
    this->monster->m_velocity = (destination - this->monster->m_position).Normalized();

    if (currentState == STATE_ATTACK || currentState == STATE_RUN || currentState == STATE_RAMPAGE)
    {
        this->monster->m_velocity *= 2.f;
    }
}

int AI_Strategy::CalculateDistance(const Vector3& MonsterPos, const Vector3& Destination)
{
	return int((Destination - MonsterPos).LengthSquared());
}

void AI_Strategy::Update()
{
    if (currentState == STATE_TRAPPED || currentState == STATE_CAPTURED || currentState == STATE_RAMPAGE)
    {
        return;
    }

	float aggressionLevel = monster->GetAggressionStat();
	float fearLevel = monster->GetFearStat();

	if (aggressionLevel > 70 && fearLevel < 50)
	{
        if (currentState != STATE_ATTACK)
        {
            SetState(STATE_ATTACK);
            std::cout << "ATTACK!";
        }
        SetAttackStateDestination();
	}
    else if (aggressionLevel < 40 && fearLevel > 60)
	{
        if (currentState != STATE_RUN)
        {
            SetState(STATE_RUN);
            std::cout << "RUN!";
        }
	}

    else if (aggressionLevel > 50 || fearLevel > 50)
    {
        if (currentState != STATE_ALERT)
        {
            SetState(STATE_ALERT);
        }
    }

    else if (currentState != STATE_RAMPAGE && currentState != STATE_BAITED && currentState != STATE_IDLE)
	{
		monster->ResetAggression();
		monster->ResetFear();
		SetState(STATE_IDLE);
        std::cout << "IDLE";
	}

    if (CheckDestinationReached())
    {
        if (currentState == STATE_IDLE) {
            SetIdleStateDestination();
        }
        else if (currentState == STATE_RUN) {
            SetRunStateDestination();
        }
    }
}

void AI_Strategy::SetIdleStateDestination()
{
    int cols = (int)(monster->m_position.x / Scene::tileSize);
    int rows = (int)(monster->m_position.z / Scene::tileSize);

    int randCol = cols + Math::RandIntMinMax(1, 5) * Math::RandIntMinMax(-1, 1);
    int randRow = rows + Math::RandIntMinMax(1, 5) * Math::RandIntMinMax(-1, 1);
    while ((randCol < 0 || randCol >= 40) || (randRow < 0 || randRow >= 40) || Scene::m_levelMap[randRow][randCol] != '0')
    {
        randCol = cols + Math::RandIntMinMax(1, 5) * Math::RandIntMinMax(-1, 1);
        randRow = rows + Math::RandIntMinMax(1, 5) * Math::RandIntMinMax(-1, 1);
    }

    //Vector3 RNGdestination;
    //int col = 40;
    //int row = 40;
    //while ((col < 0 || col >= 40) || (row < 0 || row >= 40) || Scene::m_levelMap[row][col] != '0')
    //{
    //    RNGdestination.Set(monster->m_position.x + Math::RandFloatMinMax(-20.f, 20.f), 0, monster->m_position.z + Math::RandFloatMinMax(-20.f, 20.f));
    //    col = (int)(RNGdestination.x / Scene::tileSize);
    //    row = (int)(RNGdestination.z / Scene::tileSize);
    //}

    Vector3 RNGdestination(randCol * Scene::tileSize + Scene::tileSize * 0.5f, 0, randRow * Scene::tileSize + Scene::tileSize * 0.5f);

    setDestination(RNGdestination);
}

void AI_Strategy::SetAttackStateDestination()
{
    Vector3 destination = SharedData::GetInstance()->player->GetPositionVector();
    destination.y = 0.f;
    setDestination(destination);
}

void AI_Strategy::SetRunStateDestination()
{
    Vector3 view = monster->m_position - SharedData::GetInstance()->player->GetPositionVector();
    view.y = 0.f;
    view.Normalize();

    Vector3 destination = monster->m_position + 5 * view;

    setDestination(destination);
}

void AI_Strategy::SetRampageStateDestination()
{
	Vector3 destination = SharedData::GetInstance()->player->GetPositionVector();
	destination.y = 0.f;
	setDestination(destination);
	std::cout << "DESTINATION" << std::endl;
}