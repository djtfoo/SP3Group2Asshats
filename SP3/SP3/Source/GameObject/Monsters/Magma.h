#ifndef MONSTER_MAGMA_H
#define MONSTER_MAGMA_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Monster_Magma : public Monster
{
private:
	int AggressionLevel = 0;
	int FearLevel = 0;

public:
	Monster_Magma(std::string name, int stats[]);
	virtual ~Monster_Magma();

	//Monster Movement update
	virtual void Update(double dt);
};


#endif