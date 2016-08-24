#ifndef RABBIT_H
#define RABBIT_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Monster_Rabbit : public Monster
{
private:
	int AggressionLevel = 0;
	int FearLevel = 0;

public:
	Monster_Rabbit(std::string name, int stats[]);
	virtual ~Monster_Rabbit();
	//Monster Movement update
	virtual void Update(double dt);
};

#endif