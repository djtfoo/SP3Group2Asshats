#ifndef MONSTER_FOSSIL_H
#define MONSTER_FOSSIL_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Monster_Fossil : public Monster
{
private:
	int AggressionLevel = 0;
	int FearLevel = 0;

public:
	Monster_Fossil(std::string name, int stats[]);
	virtual ~Monster_Fossil();

	//Monster Movement update
	virtual void Update(double dt);
};

#endif