#include "Golem.h"
#include "../AI_Strategy.h"

Monster_Golem::Monster_Golem(std::string name, const std::vector<int>& stats) : Monster(name, stats)
{
    m_strategy = new AI_Strategy();
    m_strategy->Init(this);
}

Monster_Golem::~Monster_Golem()
{
    if (m_strategy != NULL)
    {
        delete m_strategy;
        m_strategy = NULL;
    }
}

void Monster_Golem::Update(double dt)
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
        AggressionLevel = 10 * dt * SharedData::GetInstance()->player->GetNoiseFactor();
    }

    else if (GetHealthStat() < 50)
    {
        AggressionLevel = -10 * dt * SharedData::GetInstance()->player->GetNoiseFactor();
        FearLevel = 50 * dt * SharedData::GetInstance()->player->GetNoiseFactor();
    }

    updateStats(dt);

    //Update Strategy accordingly
    m_strategy->Update();
}

void Monster_Golem::TakeDamage(const int damage)
{
    // take lesser damage as he is a rock
    changeHealthStat(m_healthStat - 0.2f * damage);
	changeCaptureRateStat(m_captureRateStat + 0.02f * damage);

    AggressionLevel = 3.f;
    changeAggressionStat(m_aggressionStat + AggressionLevel);
}

void Monster_Golem::PlaySoundEffect()
{
	SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//RockZone//Golem.wav",
		irrklang::vec3df(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z),
		irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
		irrklang::vec3df(m_position.x, m_position.y, m_position.z));
}