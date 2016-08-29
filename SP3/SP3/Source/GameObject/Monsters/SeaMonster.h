#ifndef MONSTER_KOF_H
#define MONSTER_KOF_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Monster_SeaMonster : public Monster
{
public:
    Monster_SeaMonster(std::string name, const std::vector<int>& stats);
	virtual ~Monster_SeaMonster();

	//Monster Movement update
	virtual void Update(double dt);
    virtual void TakeDamage(const int damage);
};

#endif