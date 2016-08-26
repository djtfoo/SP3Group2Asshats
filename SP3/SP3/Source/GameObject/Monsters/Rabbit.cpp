#include "Rabbit.h"
#include "../AI_Strategy.h"

#include "../../Scene/Scene.h"

Monster_Rabbit::Monster_Rabbit(std::string name, const std::vector<int>& stats) : Monster(name, stats)
{
    m_strategy = new AI_Strategy();
    m_strategy->Init(this);
}

Monster_Rabbit::~Monster_Rabbit()
{
    if (m_strategy)
    {
        delete m_strategy;
        m_strategy = NULL;
    }
}

void Monster_Rabbit::Update(double dt)
{
    AggressionLevel = 0.f;
    FearLevel = 0.f;

    if ((m_position - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() > (8 * Scene::tileSize) * (8 * Scene::tileSize))
    {
        //reInit AggressionStat && FearStat
        ResetAggression();
        ResetFear();
    }

    //If near Player, increase fear
    else if ((m_position - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() < (5 * Scene::tileSize) * (5 * Scene::tileSize))
    {
        FearLevel = 20 * dt * SharedData::GetInstance()->player->GetNoiseFactor();
    }
    //If health < 25, increase fear
    else if (GetHealthStat() < 25)
    {
        FearLevel = 30 * dt * SharedData::GetInstance()->player->GetNoiseFactor();
    }

    if (m_strategy->GetState() == AI_Strategy::STATE_BAITED)
    {
        AggressionLevel /= 2.f;
        FearLevel /= 2.f;
    }

    changeFearStat(m_fearStat + FearLevel);

    //Update Strategy accordingly
    m_strategy->Update();
}

void Monster_Rabbit::TakeDamage(const int damage)
{
    changeHealthStat(m_healthStat - damage);

    FearLevel = 5.f;
    changeFearStat(m_fearStat + FearLevel);
}