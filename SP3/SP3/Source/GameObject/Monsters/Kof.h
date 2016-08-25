#ifndef MONSTER_KOF_H
#define MONSTER_KOF_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Monster_Kof : public Monster
{
public:
    Monster_Kof(std::string name, const std::vector<int>& stats);
	virtual ~Monster_Kof();

	//Monster Movement update
	virtual void Update(double dt);
};

#endif