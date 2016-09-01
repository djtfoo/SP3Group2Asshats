#include "SaveData.h"
#include "SharedData.h"
#include "LoadFile.h"

#include "../Scene/Zoo/SceneZoo.h"

#include <iostream>
#include <fstream>
#include <sstream>

SaveData::SaveData()
: b_saveGameExists(false)
, questCount(0)
, questActivated(false)
, playerCurrency(0)
, rockQuantity(50)
, netQuantity(50)
, baitQuantity(50)
, trapQuantity(50)
, meatQuantity(0)
, rockUpgradeLevel(0)
, netUpgradeLevel(0)
, baitUpgradeLevel(0)
, trapUpgradeLevel(0)
, grassZooSize(0)
, swampZooSize(0)
, rockZooSize(0)
, lavaZooSize(0)
{
}

SaveData::~SaveData()
{
}

void SaveData::Init(const char* file_path)
{
    b_saveGameExists = LoadFile(file_path, FILE_SAVEDATA);
}

void SaveData::ClearGame()
{
    SharedData::GetInstance()->questManager->Exit();
    SharedData::GetInstance()->questManager->Init();

    SharedData::GetInstance()->player->m_currency = 0;

    Item tempInventory[Item::NUM_TYPE] =
    {
        Item("Net", Item::TYPE_NET, 10, 100, 0),
        Item("Bait", Item::TYPE_BAIT, 10, 100, 0),
        Item("Meat", Item::TYPE_MEAT, 10, 100, 10),
        Item("Trap", Item::TYPE_TRAP, 10, 100, 10),
        Item("Rock", Item::TYPE_ROCK, 10, 100, 10)
    };

    for (unsigned i = 0; i < Item::NUM_TYPE; ++i)
    {
        SharedData::GetInstance()->player->inventory[i] = tempInventory[i];
        switch (SharedData::GetInstance()->player->inventory[i].GetItemType())
        {
        case Item::TYPE_NET:
        case Item::TYPE_BAIT:
        case Item::TYPE_TRAP:
        case Item::TYPE_ROCK:
            SharedData::GetInstance()->player->inventory[i].Add(50);
            break;

        default:
            SharedData::GetInstance()->player->inventory[i].Add(0);
            break;
        }
    }

    // Zoo enclosure sizes
    SceneZoo::grassAreaSize = 10;
    SceneZoo::swampAreaSize = 10;
    SceneZoo::rockAreaSize = 10;
    SceneZoo::fireAreaSize = 10;

    ItemProjectile::d_netCooldown = 3.0;

    // Caught monsters
    if (!SharedData::GetInstance()->player->monsterList.empty())
    {
        SharedData::GetInstance()->player->monsterList.clear();
    }
}

bool SaveData::SaveGame()
{
    saveGameData();
    b_saveGameExists = writeToSaveFile("GameData//SaveFile.csv");

    return b_saveGameExists;
}

void SaveData::LoadGame()
{
    ClearGame();

    // Quest
    if (this->questActivated)
    {
        SharedData::GetInstance()->questManager->SetQuestActive();
    }
    else
    {
        SharedData::GetInstance()->questManager->SetQuestInactive();
    }

    for (unsigned i = 1; i < this->questCount; ++i)
    {
        SharedData::GetInstance()->questManager->PopQuest();
    }

    // currency
    SharedData::GetInstance()->player->m_currency = this->playerCurrency;

    // inventory items
    for (unsigned i = 0; i < this->rockUpgradeLevel; ++i)
    {
        SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].Upgrade();
    }

    for (unsigned i = 0; i < this->netUpgradeLevel; ++i)
    {
        SharedData::GetInstance()->player->inventory[Item::TYPE_NET].Upgrade();
    }

    for (unsigned i = 0; i < this->baitUpgradeLevel; ++i)
    {
        SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].Upgrade();
    }

    for (unsigned i = 0; i < this->trapUpgradeLevel; ++i)
    {
        SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].Upgrade();
    }

    // inventory items quantity
    for (unsigned i = 0; i < 50; ++i)
    {
        SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].Use();
        SharedData::GetInstance()->player->inventory[Item::TYPE_NET].Use();
        SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].Use();
        SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].Use();
    }

    SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].Add(this->rockQuantity);
    SharedData::GetInstance()->player->inventory[Item::TYPE_NET].Add(this->netQuantity);
    SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].Add(this->baitQuantity);
    SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].Add(this->trapQuantity);
    SharedData::GetInstance()->player->inventory[Item::TYPE_MEAT].Add(this->meatQuantity);

    // Zoo enclosure sizes
    SceneZoo::grassAreaSize = this->grassZooSize;
    SceneZoo::swampAreaSize = this->swampZooSize;
    SceneZoo::rockAreaSize = this->rockZooSize;
    SceneZoo::fireAreaSize = this->lavaZooSize;

    // Caught monsters
    for (unsigned i = 0; i < monsterList.size(); ++i)
    {
        SharedData::GetInstance()->player->monsterList.push_back(monsterList[i]);
    }
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
    questActivated = SharedData::GetInstance()->questManager->IsQuestActive();
    playerCurrency = SharedData::GetInstance()->player->m_currency;

    rockQuantity = SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].GetCount();
    netQuantity = SharedData::GetInstance()->player->inventory[Item::TYPE_NET].GetCount();
    baitQuantity = SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].GetCount();
    trapQuantity = SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].GetCount();
    meatQuantity = SharedData::GetInstance()->player->inventory[Item::TYPE_MEAT].GetCount();

    rockUpgradeLevel = SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].GetCurrentUpgradeLevel();
    netUpgradeLevel = SharedData::GetInstance()->player->inventory[Item::TYPE_NET].GetCurrentUpgradeLevel();
    baitUpgradeLevel = SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].GetCurrentUpgradeLevel();
    trapUpgradeLevel = SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].GetCurrentUpgradeLevel();

    grassZooSize = SceneZoo::grassAreaSize;
    swampZooSize = SceneZoo::swampAreaSize;
    rockZooSize = SceneZoo::rockAreaSize;
    lavaZooSize = SceneZoo::fireAreaSize;

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

    // 1st content: questCount
    ss << "Quest Count," << questCount;
    fileStream << ss.str() << std::endl;

    // 2nd content: bool questActivated
    ss.str("");
    ss << "bool questActivated,";
    if (questActivated)
        ss << 1;
    else
        ss << 0;
    fileStream << ss.str() << std::endl;

    // 3rd content: currency
    ss.str("");
    ss << "Currency," << playerCurrency;
    fileStream << ss.str() << std::endl;

    // 4th - 8th content: item quantity
    ss.str("");
    ss << "Rock quantity," << rockQuantity;
    fileStream << ss.str() << std::endl;

    ss.str("");
    ss << "Net quantity," << netQuantity;
    fileStream << ss.str() << std::endl;

    ss.str("");
    ss << "Bait quantity," << baitQuantity;
    fileStream << ss.str() << std::endl;

    ss.str("");
    ss << "Trap quantity," << trapQuantity;
    fileStream << ss.str() << std::endl;

    ss.str("");
    ss << "Meat quantity," << meatQuantity;
    fileStream << ss.str() << std::endl;

    // 4th content: rock upgrade level
    ss.str("");
    ss << "Rock Level," << rockUpgradeLevel;
    fileStream << ss.str() << std::endl;

    // 5th content: net upgrade level
    ss.str("");
    ss << "Net Level," << netUpgradeLevel;
    fileStream << ss.str() << std::endl;

    // 6th content: net upgrade level
    ss.str("");
    ss << "Bait Level," << baitUpgradeLevel;
    fileStream << ss.str() << std::endl;

    // 7th content: net upgrade level
    ss.str("");
    ss << "Trap Level," << trapUpgradeLevel;
    fileStream << ss.str() << std::endl;

    // 8th content: grass area size
    ss.str("");
    ss << "Grass size," << grassZooSize;
    fileStream << ss.str() << std::endl;

    // 9th content: swamp area size
    ss.str("");
    ss << "Swamp size," << swampZooSize;
    fileStream << ss.str() << std::endl;

    // 10th content: rock area size
    ss.str("");
    ss << "Rock size," << rockZooSize;
    fileStream << ss.str() << std::endl;

    // 11th content: lava area size
    ss.str("");
    ss << "Lava size," << lavaZooSize;
    fileStream << ss.str() << std::endl;

    // 12th content: monsters caught
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