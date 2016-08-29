#include "MagmaBerzeker.h"
#include "../AI_Strategy.h"

#include "../../Scene/Scene.h"

Boss_MagmaBerzeker::Boss_MagmaBerzeker(std::string name, const std::vector<int>& stats) : Monster(name, stats)
{
	m_strategy = new AI_Strategy();
	m_strategy->Init(this);
}

Boss_MagmaBerzeker::~Boss_MagmaBerzeker()
{
	if (m_strategy)
	{
		delete m_strategy;
		m_strategy = NULL;
	}
}

void Boss_MagmaBerzeker::Update(double dt)
{
	AggressionLevel = 0.f;
	FearLevel = 0.f;

	if ((m_position - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() > (8 * Scene::tileSize) * (8 * Scene::tileSize))
	{
		//reInit AggressionStat && FearStat
		ResetAggression();
		ResetFear();
	}

	//If near Player, increase aggro
	else if ((m_position - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() < (5 * Scene::tileSize) * (5 * Scene::tileSize))
	{
		//std::cout << "increasing aggression...";
		AggressionLevel = 75 * dt * SharedData::GetInstance()->player->GetNoiseFactor();
	}
	//If health < 25, decrease aggro
	else if (GetHealthStat() < 25)
	{
		AggressionLevel = -25 * dt * SharedData::GetInstance()->player->GetNoiseFactor();
		//FearLevel = 30 * dt;
	}

	if (m_strategy->GetState() == AI_Strategy::STATE_BAITED)
	{
		AggressionLevel /= 2.f;
		FearLevel /= 2.f;
	}

	changeAggressionStat(m_aggressionStat + AggressionLevel);
	//changeFearStat(m_fearStat + FearLevel);

	//Update Strategy accordingly
	m_strategy->Update();
}

void Boss_MagmaBerzeker::TakeDamage(const int damage)
{
	changeHealthStat(m_healthStat - damage);

	AggressionLevel = 5.f;
	changeAggressionStat(m_aggressionStat + AggressionLevel);
}