#ifndef MONSTER_FIRE_BUGS_H
#define MONSTER_FIRE_BUGS_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Monster_FireBugs : public Monster
{
private:
	int AggressionLevel = 0;
	int FearLevel = 0;

public:
	Monster_FireBugs(std::string name, int stats[]);
	virtual ~Monster_FireBugs();

	//Monster Movement update
	virtual void Update(double dt);
};

#endif