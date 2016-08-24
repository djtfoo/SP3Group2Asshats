#ifndef MONSTER_KOF_H
#define MONSTER_KOF_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Monster_Kof : public Monster
{
private:
	int AggressionLevel = 0;
	int FearLevel = 0;

public:
	Monster_Kof(std::string name, int stats[]);
	virtual ~Monster_Kof();

	//Monster Movement update
	virtual void Update(double dt);
};

#endif