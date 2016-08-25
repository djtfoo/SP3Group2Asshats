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

void AI_Strategy::SetState(AI_Strategy::STRATEGY_MODE currentState)
{
	this->currentState = currentState;

    // change the destination
}

bool AI_Strategy::CheckDestinationReached()
{
    if ((monster->m_position - monster->m_destination).LengthSquared() < (0.5f * Scene::tileSize) * (0.5f * Scene::tileSize))
        return true;

    return false;
}

void AI_Strategy::SetDestination(const Vector3& destination)
{
    this->monster->m_destination = destination;
    this->monster->m_velocity = (destination - this->monster->m_position).Normalized();
}

int AI_Strategy::CalculateDistance(const Vector3& MonsterPos, const Vector3& Destination)
{
	return int((Destination - MonsterPos).LengthSquared());
}

void AI_Strategy::Update()
{
    //std::cout << " AI UPDATE ";
	float m_aggressionLevel = monster->GetAggressionStat();
	float m_fearLevel = monster->GetFearStat();
    
	if (m_aggressionLevel > 60 && m_fearLevel < 50)
	{
        if (currentState != STATE_ATTACK)
        {
            SetState(STATE_ATTACK);
            std::cout << "ATTACK!";
        }
	}
    else if (m_aggressionLevel < 40 && m_fearLevel > 50)
	{
        if (currentState != STATE_RUN)
        {
            SetState(STATE_RUN);
            std::cout << "RUN!";
        }
	}
	//if (monster collide with trap)
	//{
	//	SetState(TRAPPED);
	//	if(timer > 5)
	//	SetState(Prev State);
	//}
    else if (currentState != STATE_IDLE)
	{
		SetState(STATE_IDLE);
        std::cout << "IDLE";
	}

    if (CheckDestinationReached())
    {
        if (currentState == STATE_IDLE) {
            SetIdleDestination();
        }
    }
}

void AI_Strategy::SetIdleDestination()
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

    SetDestination(RNGdestination);
}