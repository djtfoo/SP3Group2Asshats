/******************************************************************************/
/*!
\file	Monster.cpp
\author Foo Jing Ting
\par	email: 152856H@mymail.nyp.edu.sg
\brief
Class that defines a monster's variables and statistics
*/
/******************************************************************************/
#include "Monster.h"
#include "MonsterFactory.h"

Monster::Monster(std::string name, int stats[]) : m_name(name), m_originalAggression(stats[2]), m_originalFear(stats[3])
{
    // idea: Monster(std::string name);
    // based on name, retrieve stats from a struct/array that already initialised with the stats from the text files

    this->m_healthStat = stats[0];
    this->m_captureRateStat = stats[1];
    this->m_aggressionStat = stats[2];
    this->m_fearStat = stats[3];
    //this->m_strategy = (AI_Strategy *)(stats[4]);  //aggressive, bossfairy, etc.?
}

Monster::~Monster()
{
}

std::string Monster::GetName()
{
    return m_name;
}

int Monster::GetHealthStat()
{
    return m_healthStat;
}

int Monster::GetCaptureRateStat()
{
    return m_captureRateStat;
}

int Monster::GetAggressionStat()
{
    return m_aggressionStat;
}

int Monster::GetFearStat()
{
    return m_fearStat;
}

bool Monster::CheckCapture()
{
    return false;
}

void Monster::SetPosition(Vector3 m_position)
{
	this->m_position = m_position;
}

Vector3 Monster::GetPosition()
{
	return m_position;
}

void Monster::move()
{
    m_position += m_velocity;   // * dt
}

void Monster::changeHealthStat(const int newHealth)
{
    m_healthStat = newHealth;
}

void Monster::changeAggressionStat(const int newAggression)
{
    m_aggressionStat = newAggression;
}

void Monster::changeFearStat(const int newFear)
{
    m_fearStat = newFear;
}

void Monster::changeCaptureRateStat(const int newCaptureRate)
{
    m_captureRateStat = newCaptureRate;
}

void Monster::ResetAggression()
{
	m_aggressionStat = m_originalAggression;
}

void Monster::ResetFear()
{
	m_fearStat = m_originalFear;
}