#ifndef MONSTER_GOLEM_H
#define MONSTER_GOLEM_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Monster_Golem : public Monster
{
public:
    Monster_Golem(std::string name, int stats[]);
    virtual ~Monster_Golem();

	//Monster Movement update
    virtual void Update(double dt);
};

#endif