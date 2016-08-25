#ifndef MONSTER_MAGMA_H
#define MONSTER_MAGMA_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Monster_Magma : public Monster
{
public:
    Monster_Magma(std::string name, const std::vector<int>& stats);
	virtual ~Monster_Magma();

	//Monster Movement update
	virtual void Update(double dt);
};


#endif