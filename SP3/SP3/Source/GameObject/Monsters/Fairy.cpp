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
}

void Boss_Fairy::TakeDamage(const int damage)
{
    changeHealthStat(m_healthStat - damage);
}

void Boss_Fairy::PlaySoundEffect()
{
	SharedData::GetInstance()->sound->playSoundEffect3D("Sound//Bird2.wav",
		irrklang::vec3df(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z),
		irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
		irrklang::vec3df(m_position.x, m_position.y, m_position.z));
}