#ifndef MONSTER_BIRD_H
#define MONSTER_BIRD_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Monster_Bird : public Monster
{
private:
	int AggressionLevel = 0;
	int FearLevel = 0;

public:
	Monster_Bird(std::string name, int stats[]);
	virtual ~Monster_Bird();
	//Monster Movement update
	virtual void Update(double dt);
};

#endif 