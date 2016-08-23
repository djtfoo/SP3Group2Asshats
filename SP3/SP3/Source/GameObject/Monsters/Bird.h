#ifndef MONSTER_BIRD_H
#define MONSTER_BIRD_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Monster_Bird : public Monster
{
public:
    Monster_Bird(std::string name, int stats[]) : Monster(name, stats)
	{
		m_strategy = NULL;
	}
	virtual ~Monster_Bird()
	{
		if (m_strategy != NULL)
		{
			delete m_strategy;
			m_strategy = NULL;
		}
	}

	void ChangeStrategy(AI_Strategy* newAI, bool bDelete)
	{
		if (bDelete == true)
		{
			if (m_strategy != NULL)
			{
				delete m_strategy;
				m_strategy = NULL;
			}
		}
		m_strategy = newAI;
	}

	//Monster Movement update
	virtual void Update(double dt)
	{
		// Do distance check between player, bait, traps
		// Do aggression change and fear change respectively
		if ((GetPosition() - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() < 8)
		{
			changeAggressionStat(5);
			changeFearStat(-5);
		}
		else if ((GetPosition() - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() > 8)
		{
			GetAggressionStat();
			GetFearStat();
		}
		if (GetHealthStat() < 40)
		{
			changeAggressionStat(-5);
			changeFearStat(5);
		}
	}

};

#endif 