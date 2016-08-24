#include "MukBoss.h"
#include "../AI_Strategy.h"

Boss_MukBoss::Boss_MukBoss(std::string name, int stats[]) : Monster(name, stats)
{
    m_strategy = NULL;
}

Boss_MukBoss::~Boss_MukBoss()
{
    if (m_strategy != NULL)
    {
        delete m_strategy;
        m_strategy = NULL;
    }
}

void Boss_MukBoss::Update(double dt)
{
	if ((GetPosition() - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() > 24)
	{
		//reInit AggressionStat && FearStat
		ResetAggression();
		ResetFear();
	}
	//If near Player, increase aggro
	if ((GetPosition() - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() < 16)
	{
		AggressionLevel = 20;
		changeAggressionStat(m_aggressionStat + AggressionLevel);
		if (AggressionLevel >= 100)
		{
			AggressionLevel = 100;
		}
	}
	//If health < 25, decrease aggro, increase fear 
	if (GetHealthStat() < 25)
	{
		AggressionLevel = -25;
		FearLevel = 20;
		changeAggressionStat(m_aggressionStat + AggressionLevel);
		changeFearStat(m_fearStat + FearLevel);
		if (AggressionLevel <= 0)
		{
			AggressionLevel = 0;
		}
		if (FearLevel >= 100)
		{
			FearLevel = 100;
		}
	}

	//Get Aggression Stat and Fear Stat
	GetAggressionStat();
	GetFearStat();

	//Update Strategy accordingly
	m_strategy->Update();
}