/******************************************************************************/
/*!
\file	MonsterFactory.cpp
\author Foo Jing Ting
\par	email: 152856H@mymail.nyp.edu.sg
\brief
Class that creates Monsters inside a scene
*/
/******************************************************************************/
#include "MonsterFactory.h"
#include "../General/LoadFile.h"

#include "Monsters/Bird.h"
#include "Monsters/Fairy.h"
#include "Monsters/FireBugs.h"
#include "Monsters/Fossil.h"
#include "Monsters/Golem.h"
#include "Monsters/Grimejam.h"
#include "Monsters/Kof.h"
#include "Monsters/Magma.h"
#include "Monsters/MagmaBerzeker.h"
#include "Monsters/MukBoss.h"
#include "Monsters/Rabbit.h"
#include "Monsters/RockSnake.h"

MonsterFactory::monsterStatsMap MonsterFactory::m_monsterStatsList = {};

void MonsterFactory::LoadMonsterData(const char* file_path)
{
    LoadFile(file_path, FILE_MONSTERDATA);
}

Monster* MonsterFactory::CreateMonster(const std::string name)
{
    Monster* createdMonster = 0;

    monsterStatsMap::iterator it = MonsterFactory::m_monsterStatsList.find(name);

    // Grass Zone
    if (name == "Bird")
        createdMonster = new Monster_Bird(name, it->second);
    else if (name == "Rabbit")
        createdMonster = new Monster_Rabbit(name, it->second);
    else if (name == "Fairy")
        createdMonster = new Boss_Fairy(name, it->second);

    // Water Zone
    else if (name == "Grimejam")
        createdMonster = new Monster_Grimejam(name, it->second);
    else if (name == "Kof")
        createdMonster = new Monster_Kof(name, it->second);
    else if (name == "MukBoss")
        createdMonster = new Boss_MukBoss(name, it->second);

    // Rock Zone
    else if (name == "Fossil")
        createdMonster = new Monster_Fossil(name, it->second);
    else if (name == "Golem")
        createdMonster = new Monster_Golem(name, it->second);
    else if (name == "RockSnake")
        createdMonster = new Boss_RockSnake(name, it->second);

    // Fire Zone
    else if (name == "FireBug")
        createdMonster = new Monster_FireBugs(name, it->second);
    else if (name == "Magma")
        createdMonster = new Monster_Magma(name, it->second);
    else if (name == "MagmaBerzeker")
        createdMonster = new Boss_MagmaBerzeker(name, it->second);


    return createdMonster;
}

void MonsterFactory::AddToMap(std::string name, const std::vector<int>& stats)
{
    m_monsterStatsList.insert(std::pair<std::string, std::vector<int>>(name, stats));
}