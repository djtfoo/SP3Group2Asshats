#ifndef MONSTER_BOSS_ROCKSNAKE_H
#define MONSTER_BOSS_ROCKSNAKE_H

#include "../Monster.h"
#include "../../General/SharedData.h"

class Boss_RockSnake : public Monster
{
public:
    Boss_RockSnake(std::string name, int stats[]);
    virtual ~Boss_RockSnake();

	//Monster Movement update
    virtual void Update(double dt);
};



#endif