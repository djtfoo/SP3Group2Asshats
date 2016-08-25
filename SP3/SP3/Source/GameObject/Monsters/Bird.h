#ifndef MONSTER_BIRD_H
#define MONSTER_BIRD_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Monster_Bird : public Monster
{
public:
	Monster_Bird(std::string name, const std::vector<int>& stats);
	virtual ~Monster_Bird();

	//Monster Movement update
	virtual void Update(double dt);
};

#endif 