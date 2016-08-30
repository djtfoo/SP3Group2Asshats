#include "Bird.h"
#include "../AI_Strategy.h"

#include "../../Scene/Scene.h"
#include "../../General/SharedData.h"

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

	updateStats(dt);

	//Update Strategy accordingly
    m_strategy->Update();
}

void Monster_Bird::TakeDamage(const int damage)
{
    changeHealthStat(m_healthStat - damage);
	changeCaptureRateStat(m_captureRateStat + 0.2f * damage);
    
    AggressionLevel = 5.f;
    changeAggressionStat(m_aggressionStat + AggressionLevel);
}

void Monster_Bird::PlaySoundEffect()
{
	SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//Bird2.wav",
		irrklang::vec3df(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z),
		irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
		irrklang::vec3df(m_position.x, m_position.y, m_position.z));
}