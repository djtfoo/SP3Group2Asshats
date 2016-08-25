#include "Bird.h"
#include "../AI_Strategy.h"

#include "../../Scene/Scene.h"

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
    if ((m_position - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() > (8 * Scene::tileSize) * (8 * Scene::tileSize))
	{
		//reInit AggressionStat && FearStat
		ResetAggression();
		ResetFear();
	}
	//If near Player, increase aggro
    else if ((m_position - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() < (4 * Scene::tileSize) * (4 * Scene::tileSize))
    {
        //std::cout << "increasing aggression...";
        AggressionLevel = 30 * dt;
        changeAggressionStat(m_aggressionStat + AggressionLevel);
    }
	//If health < 25, decrease aggro, increase fear
    else if (GetHealthStat() < 25)
    {
        AggressionLevel = -20 * dt;
        FearLevel = 30 * dt;
        changeAggressionStat(m_aggressionStat + AggressionLevel);
        changeFearStat(m_fearStat + FearLevel);
    }

	//Update Strategy accordingly
    m_strategy->Update();

    //std::cout << "Health:" << m_healthStat << " ";
    //std::cout << "CapRate:" << m_captureRateStat << " ";
    //std::cout << "Aggro:" << m_aggressionStat << " ";
    //std::cout << "Fear:" << m_fearStat << std::endl;
}
