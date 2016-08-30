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

	Monster::updateStats(dt);

    //Update Strategy accordingly
    m_strategy->Update();
}

void Monster_Rabbit::TakeDamage(const int damage)
{
    changeHealthStat(m_healthStat - damage);

    FearLevel = 5.f;
    changeFearStat(m_fearStat + FearLevel);
}

void Monster_Rabbit::PlaySoundEffect()
{
	SharedData::GetInstance()->sound->playSoundEffect3D("Sound//Rabbit.wav",
		irrklang::vec3df(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z),
		irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
		irrklang::vec3df(m_position.x, m_position.y, m_position.z));
}