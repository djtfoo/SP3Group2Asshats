#ifndef MONSTER_GRIMEJAM_H
#define MONSTER_GRIMEJAM_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Monster_Grimejam : public Monster
{
private:
	int AggressionLevel = 0;
	int FearLevel = 0;

public:
	Monster_Grimejam(std::string name, int stats[]);
	virtual ~Monster_Grimejam();

	//Monster Movement update
	virtual void Update(double dt);
};

#endif