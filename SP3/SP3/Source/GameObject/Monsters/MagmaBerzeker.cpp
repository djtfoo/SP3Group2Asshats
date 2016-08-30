#include "MagmaBerzeker.h"
#include "../AI_Strategy.h"

#include "../../Scene/Scene.h"

Boss_MagmaBerzeker::Boss_MagmaBerzeker(std::string name, const std::vector<int>& stats) : Monster(name, stats)
{
	m_strategy = new AI_Strategy();
	m_strategy->Init(this);
}

Boss_MagmaBerzeker::~Boss_MagmaBerzeker()
{
	if (m_strategy)
	{
		delete m_strategy;
		m_strategy = NULL;
	}
}

void Boss_MagmaBerzeker::Update(double dt)
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
		AggressionLevel = 70 * dt * SharedData::GetInstance()->player->GetNoiseFactor();
	}
	//If health < 25, decrease aggro
	else if (GetHealthStat() < 25)
	{
		AggressionLevel = -25 * dt * SharedData::GetInstance()->player->GetNoiseFactor();
		//FearLevel = 30 * dt;
	}

	Monster::updateStats(dt);

	//Update Strategy accordingly
	m_strategy->Update();
}

void Boss_MagmaBerzeker::TakeDamage(const int damage)
{
	changeHealthStat(m_healthStat - damage);
	changeCaptureRateStat(m_captureRateStat + 0.05f * damage);

	AggressionLevel = 5.f;
	changeAggressionStat(m_aggressionStat + AggressionLevel);
}

void Boss_MagmaBerzeker::PlaySoundEffect()
{
	SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//Bird2.wav",
		irrklang::vec3df(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z),
		irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
		irrklang::vec3df(m_position.x, m_position.y, m_position.z));
}