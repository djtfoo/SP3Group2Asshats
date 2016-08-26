#ifndef MONSTER_BOSS_FAIRY_H
#define MONSTER_BOSS_FAIRY_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Boss_Fairy : public Monster
{
public:
    Boss_Fairy(std::string name, const std::vector<int>& stats);
	virtual ~Boss_Fairy();

	//Monster Movement update
	virtual void Update(double dt);
    virtual void TakeDamage(const int damage);
};

#endif 