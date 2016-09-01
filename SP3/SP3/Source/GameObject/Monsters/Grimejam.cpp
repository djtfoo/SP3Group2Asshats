#include "Grimejam.h"
#include "../AI_Strategy.h"

Monster_Grimejam::Monster_Grimejam(std::string name, const std::vector<int>& stats) : Monster(name, stats)
{
    m_strategy = new AI_Strategy();
    m_strategy->Init(this);
}

Monster_Grimejam::~Monster_Grimejam()
{
    if (m_strategy)
    {
        delete m_strategy;
        m_strategy = NULL;
    }
}

void Monster_Grimejam::Update(double dt)
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
        AggressionLevel = 40 * dt * SharedData::GetInstance()->player->GetNoiseFactor();
        FearLevel = 20 * dt * SharedData::GetInstance()->player->GetNoiseFactor();
    }

    else if (GetHealthStat() < 35)
    {
        AggressionLevel = -20 * dt * SharedData::GetInstance()->player->GetNoiseFactor();
        FearLevel = 50 * dt * SharedData::GetInstance()->player->GetNoiseFactor();
    }

    updateStats(dt);

    //Update Strategy accordingly
    m_strategy->Update();
}

void Monster_Grimejam::TakeDamage(const int damage)
{
    changeHealthStat(m_healthStat - damage);
	changeCaptureRateStat(m_captureRateStat + 0.2f * damage);

    FearLevel = 5.f;
    changeFearStat(m_fearStat + FearLevel);
}

void Monster_Grimejam::PlaySoundEffect()
{
	SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//SwampZone//Grimejam.wav",
		irrklang::vec3df(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z),
		irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
		irrklang::vec3df(m_position.x, m_position.y, m_position.z));
}