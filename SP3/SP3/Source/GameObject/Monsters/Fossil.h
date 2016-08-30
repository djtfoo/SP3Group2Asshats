#ifndef MONSTER_FOSSIL_H
#define MONSTER_FOSSIL_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Monster_Fossil : public Monster
{
public:
    Monster_Fossil(std::string name, const std::vector<int>& stats);
	virtual ~Monster_Fossil();

	//Monster Movement update
	virtual void Update(double dt);
    virtual void TakeDamage(const int damage);
	virtual void PlaySoundEffect();
};

#endif