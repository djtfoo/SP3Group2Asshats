#ifndef MONSTER_BOSS_FAIRY_H
#define MONSTER_BOSS_FAIRY_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Boss_Fairy : public Monster
{
private:
	int AggressionLevel = 0;
	int FearLevel = 0;

public:
	Boss_Fairy(std::string name, int stats[]);
	virtual ~Boss_Fairy();

	//Monster Movement update
	virtual void Update(double dt);
};


#endif 