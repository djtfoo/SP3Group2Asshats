#include "SaveData.h"
#include "SharedData.h"
#include "LoadFile.h"

#include <iostream>
#include <fstream>
#include <sstream>

SaveData::SaveData()
: questCount(0)
, questActivated(false)
, playerCurrency(0)
, rockUpgradeLevel(0)
, netUpgradeLevel(0)
, baitUpgradeLevel(0)
, trapUpgradeLevel(0)
{
}

SaveData::~SaveData()
{
}

void SaveData::Init(const char* file_path)
{
    LoadFile(file_path, FILE_SAVEDATA);
}

bool SaveData::SaveGame()
{
    saveGameData();
    return writeToSaveFile("GameData//SaveFile.csv");
}

void SaveData::saveGameData()
{
    //unsigned int questCount;    // current quest player is at
    //bool questActivated; // whether player started the quest
    //int playerCurrency;
    //unsigned RockUpgradeLevel;
    //unsigned NetUpgradeLevel;
    //unsigned BaitUpgradeLevel;
    //unsigned TrapUpgradeLevel;
    //std::vector<std::string> monsterList;

    questCount = SharedData::GetInstance()->questManager->GetCurrentQuest()->GetSerialNumber();
    //questActivated = SharedData::GetInstance()->questManager->
    playerCurrency = SharedData::GetInstance()->player->m_currency;
    rockUpgradeLevel = SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].GetCurrentUpgradeLevel();
    netUpgradeLevel = SharedData::GetInstance()->player->inventory[Item::TYPE_NET].GetCurrentUpgradeLevel();
    baitUpgradeLevel = SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].GetCurrentUpgradeLevel();
    trapUpgradeLevel = SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].GetCurrentUpgradeLevel();

    // clear monsterList vector if it is not empty
    if (!SharedData::GetInstance()->player->monsterList.empty())
    {
        SharedData::GetInstance()->player->monsterList.clear();
    }

    for (unsigned i = 0; i < SharedData::GetInstance()->player->monsterList.size(); ++i)
    {
        monsterList.push_back(SharedData::GetInstance()->player->monsterList[i]);
    }
}

bool SaveData::writeToSaveFile(const char* file_path)
{
    std::ofstream fileStream;
    fileStream.open(file_path, std::ofstream::out);
    if (!fileStream.is_open()) {
        std::cout << "Impossible to open " << file_path << ". Are you in the right directory ?\n";
        return false;
    }

    std::stringstream ss;

    // first content: questCount
    ss << "Quest Count," << questCount;
    fileStream << ss.str() << std::endl;

    // second content: bool questActivated
    ss.str("");
    ss << "bool questActivated," << (int)(questActivated);
    fileStream << ss.str() << std::endl;

    // third content: currency
    ss.str("");
    ss << "Currency," << playerCurrency;
    fileStream << ss.str() << std::endl;

    // fourth content: rock upgrade level
    ss.str("");
    ss << "Rock Level," << rockUpgradeLevel;
    fileStream << ss.str() << std::endl;

    // fifth content: net upgrade level
    ss.str("");
    ss << "Net Level," << netUpgradeLevel;
    fileStream << ss.str() << std::endl;

    // sixth content: net upgrade level
    ss.str("");
    ss << "Bait Level," << baitUpgradeLevel;
    fileStream << ss.str() << std::endl;

    // seventh content: net upgrade level
    ss.str("");
    ss << "Trap Level," << trapUpgradeLevel;
    fileStream << ss.str() << std::endl;

    // eighth content: monsters caught
    // fifth content: net upgrade level
    ss.str("");
    ss << "Monsters Caught,";
    for (unsigned i = 0; i < monsterList.size(); ++i)
    {
        ss << monsterList[i] << ",";
    }
    ss << "end,";
    fileStream << ss.str();

    // Close the file
    fileStream.close();
    return true;
}