#include "Fairy.h"
#include "../AI_Strategy.h"

#include "../../Scene/Scene.h"

Boss_Fairy::Boss_Fairy(std::string name, const std::vector<int>& stats) : Monster(name, stats)
{
    m_strategy = new AI_Strategy();
    m_strategy->Init(this);
}

Boss_Fairy::~Boss_Fairy()
{
    if (m_strategy)
    {
        delete m_strategy;
        m_strategy = NULL;
    }
}

void Boss_Fairy::Update(double dt)
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
        AggressionLevel = 40 * dt * SharedData::GetInstance()->player->GetNoiseFactor();
    }
    //If health < 25, decrease aggro
    else if (GetHealthStat() < 25)
    {
        AggressionLevel = -30 * dt * SharedData::GetInstance()->player->GetNoiseFactor();
        //FearLevel = 30 * dt;
    }

	Monster::updateStats(dt);

    //Update Strategy accordingly
    m_strategy->Update();

    //if ((m_position - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() > 24)
	//{
	//	//reInit AggressionStat && FearStat
	//	ResetAggression();
	//	ResetFear();
	//}
	////If near Player, increase aggro
    //if ((m_position - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() < 16)
	//{
	//	AggressionLevel = 15;
	//	changeAggressionStat(m_aggressionStat + AggressionLevel);
	//	if (AggressionLevel >= 100)
	//	{
	//		AggressionLevel = 100;
	//	}
	//}
	////If health < 25, decrease aggro, increase fear 
	//if (GetHealthStat() < 25)
	//{
	//	AggressionLevel = -15;
	//	FearLevel = 50;
	//	changeAggressionStat(m_aggressionStat + AggressionLevel);
	//	changeFearStat(m_fearStat + FearLevel);
	//	if (AggressionLevel <= 0)
	//	{
	//		AggressionLevel = 0;
	//	}
	//	if (FearLevel >= 100)
	//	{
	//		FearLevel = 100;
	//	}
	//}
    //
	////Get Aggression Stat and Fear Stat
	//GetAggressionStat();
	//GetFearStat();
    //
	////Update Strategy accordingly
	//m_strategy->Update();
}

void Boss_Fairy::TakeDamage(const int damage)
{
    changeHealthStat(m_healthStat - damage);
}