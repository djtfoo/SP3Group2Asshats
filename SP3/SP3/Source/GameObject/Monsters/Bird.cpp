#include "Bird.h"
#include "../AI_Strategy.h"

Monster_Bird::Monster_Bird(std::string name, const std::vector<int>& stats) : Monster(name, stats)
{
    m_strategy = new AI_Strategy();
    m_strategy->Init(this);
}

Monster_Bird::~Monster_Bird()
{
    if (m_strategy)
    {
        delete m_strategy;
        m_strategy = NULL;
    }
}

void Monster_Bird::Update(double dt)
{
	if ((m_position - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() > 24 * 24)
	{
		//reInit AggressionStat && FearStat
		ResetAggression();
		ResetFear();
	}
	//If near Player, increase aggro
    else if ((m_position - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() < 10 * 10)
    {
        //std::cout << "increasing aggression...";
        AggressionLevel = 10;
        changeAggressionStat(m_aggressionStat + AggressionLevel);
        if (AggressionLevel >= 100)
        {
            AggressionLevel = 100;
        }
        std::cout << "KEK";
    }
	//If health < 25, decrease aggro, increase fear
    else if (GetHealthStat() < 25)
    {
        AggressionLevel = -20;
        FearLevel = 50;
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

	//Update Strategy accordingly
    m_strategy->Update();

    std::cout << "Health:" << m_healthStat << " ";
    std::cout << "CapRate:" << m_captureRateStat << " ";
    std::cout << "Aggro:" << m_aggressionStat << " ";
    std::cout << "Fear:" << m_fearStat << std::endl;
}
