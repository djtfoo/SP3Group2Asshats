#ifndef RABBIT_H
#define RABBIT_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Monster_Rabbit : public Monster
{
public:
    Monster_Rabbit(std::string name, const std::vector<int>& stats);
	virtual ~Monster_Rabbit();

	//Monster Movement update
	virtual void Update(double dt);
    virtual void TakeDamage(const int damage);
	virtual void PlaySoundEffect();
};

#endif