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
#include "AI_Strategy.h"
#include "../General/SharedData.h"

Monster::Monster(std::string name, const std::vector<int>& stats) : m_name(name), m_originalAggression(stats[2]), m_originalFear(stats[3])
{
    AggressionLevel = 0.f;
    FearLevel = 0.f;

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

float Monster::GetCaptureRateStat()
{
    return m_captureRateStat;
}

float Monster::GetAggressionStat()
{
    return m_aggressionStat;
}

float Monster::GetFearStat()
{
    return m_fearStat;
}

bool Monster::CheckCapture()
{
    return false;
}

void Monster::move()
{
    m_position += m_velocity;   // * dt
}

void Monster::changeHealthStat(const int newHealth)
{
    m_healthStat = newHealth;
    m_healthStat = Math::Clamp(m_healthStat, 0, 100);
}

void Monster::changeAggressionStat(const float newAggression)
{
    m_aggressionStat = newAggression;
    m_aggressionStat = Math::Clamp(m_aggressionStat, 0.f, 100.f);
}

void Monster::changeFearStat(const float newFear)
{
    m_fearStat = newFear;
    m_fearStat = Math::Clamp(m_fearStat, 0.f, 100.f);
}

void Monster::changeCaptureRateStat(const float newCaptureRate)
{
    m_captureRateStat = newCaptureRate;
    m_captureRateStat = Math::Clamp(m_captureRateStat, 0.f, 100.f);
}

void Monster::ResetAggression()
{
	m_aggressionStat = m_originalAggression;
}

void Monster::ResetFear()
{
	m_fearStat = m_originalFear;
}

void Monster::GetTrapped()
{
    m_strategy->SetState(AI_Strategy::STATE_TRAPPED);
}

void Monster::GetCaptured()
{
    m_strategy->SetState(AI_Strategy::STATE_CAPTURED);
}

void Monster::GetBaited(const Vector3& baitPos)
{
    m_strategy->SetState(AI_Strategy::STATE_BAITED);
    m_strategy->SetDestination(baitPos);
}

void Monster::SetIdleState()
{
    m_strategy->SetState(AI_Strategy::STATE_IDLE);
}

int Monster::GetStrategyState()
{
    return static_cast<int>(m_strategy->GetState());
}

void Monster::AttackPlayer()
{
    SharedData::GetInstance()->player->TakeDamage(m_originalAggression * 0.5f);
}