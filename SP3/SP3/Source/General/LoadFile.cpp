/******************************************************************************/
/*!
\file	LoadFile.cpp
\author Foo Jing Ting
\par	email: 152856H@mymail.nyp.edu.sg
\brief
Function to read CSV files for game data
*/
/******************************************************************************/
#include <iostream>
#include <sstream>
#include "LoadFile.h"

#include "../GameObject/MonsterFactory.h"
#include "../Scene/Scene.h"
#include "SharedData.h"

bool LoadFile(const char* file_path, FILE_TYPE file_type)
{
    std::ifstream fileStream(file_path, std::ios::binary);
    if (!fileStream.is_open()) {
        std::cout << "Impossible to open " << file_path << ". Are you in the right directory ?\n";
        return false;
    }

    int numOfLines = 0;

    std::string line;
    while (!fileStream.eof())
    {
        std::getline(fileStream, line);
        if (line[0] == '#' || line == "")   // empty line OR comment
            continue;

        ++numOfLines;
    }

    fileStream.clear();
    fileStream.seekg(0, std::ios::beg);

    switch (file_type)
    {
    case FILE_MONSTERDATA:
        //MonsterFactory::SetMonsterTypesQuantity(numOfLines);
        LoadMonsterData(fileStream);
        break;

    case FILE_LEVELGENERATIONDATA:
        LoadLevelGenerationData(fileStream);
        break;

    case FILE_LEVELMAPDATA:
        LoadLevelMapData(fileStream);
        break;

    case FILE_QUESTDATA:
        LoadQuestData(fileStream);
        break;

    case FILE_SAVEDATA:
        LoadSaveData(fileStream);
        break;
    }

    fileStream.close();

    return true;
}

void LoadMonsterData(std::ifstream& fileStream)
{
    while (!fileStream.eof())
    {
        std::string line;
        std::getline(fileStream, line);

        if (line == "" || line[0] == '#')   // empty line OR comment
            continue;

        std::stringstream dataStream(line);
        std::string data;
        std::getline(dataStream, data, ',');

        // first content is name of monster
        std::string tempName = data;

        // remaining content are monster stats: health, capture rate, aggression, fear, speed, selling price
        std::vector<int> stats;
        for (int i = 0; i < 6; ++i)
        {
            std::getline(dataStream, data, ',');
            stats.push_back(std::stoi(data));
        }

        // push into map
        MonsterFactory::AddToMap(tempName, stats);
    }

}

void LoadLevelGenerationData(std::ifstream& fileStream)
{
    while (!fileStream.eof())
    {
        std::string line;
        std::getline(fileStream, line);

        if (line == "" || line[0] == '#')   // empty line OR comment
            continue;

        std::stringstream dataStream(line);
        std::string data;

        // first content is name of tile - ignore
        // second content is zone it's from - ignore
        std::getline(dataStream, data, ',');
        std::getline(dataStream, data, ',');

        // third content is tile count
        std::getline(dataStream, data, ',');
        char tempTileCount = data[0];

        if (tempTileCount == '0')
            continue;

        // remainder is components
        std::vector<COMPONENTS> tempComponents;
        std::getline(dataStream, data, ',');
        for (std::string prevData = ""; prevData != data; std::getline(dataStream, data, ','))
        {
            COMPONENTS tempComponent = ConvertStringToComponent(data);
            if (tempComponent != COMPONENT_NONE) {
                tempComponents.push_back(tempComponent);
            }
            prevData = data;
        }

        // push into map
        Scene::AddToMap(tempTileCount, AssignMeshType(tempTileCount - 65), tempComponents);
    }
}

void LoadQuestData(std::ifstream& fileStream)
{
    while (!fileStream.eof())
    {
        std::string line;
        std::getline(fileStream, line);

        if (line == "" || line[0] == '#')   // empty line OR comment
            continue;

        std::stringstream dataStream(line);
        std::string data;

        // first content is quest serial number
        std::getline(dataStream, data, ',');
        int tempSerialNum = std::stoi(data);

        // second content is quest name
        std::getline(dataStream, data, ',');
        std::string tempQuestName = data;

        // third content is required monster
        std::getline(dataStream, data, ',');
        std::string tempReqMonster = data;

        // fourth content is required monster quantity
        std::getline(dataStream, data, ',');
        int tempMonsterQuantity = std::stoi(data);

        // fifth content is the zone the quest is at
        std::getline(dataStream, data);
        std::string tempZone = data;

        // push into queue
        SharedData::GetInstance()->questManager->AddToQueue(new Quest(tempSerialNum, tempQuestName, tempReqMonster, tempMonsterQuantity, tempZone));
    }

}

GraphicsLoader::GEOMETRY_TYPE AssignMeshType(int num)
{
    GraphicsLoader::GEOMETRY_TYPE list[GraphicsLoader::NUM_GEOMETRY] = {    // legend size
        GraphicsLoader::GEO_GRASS1,     //Grass
        GraphicsLoader::GEO_GRASS2,     //Grass
        GraphicsLoader::GEO_TREE1,      //Grass
        GraphicsLoader::GEO_TREE2,      //Grass
        GraphicsLoader::GEO_SWAMP_PLANT,    //Swamp
        GraphicsLoader::GEO_SWAMP_DEADTREE, //Swamp
		GraphicsLoader::GEO_MONEY_TREE,     //Grass
		GraphicsLoader::GEO_RED_CRYSTAL,    //Lava
        GraphicsLoader::GEO_SWAMP_ROOT,     //Swamp
        GraphicsLoader::GEO_VOLCANO,    //Lava
        GraphicsLoader::GEO_ROCK_BUSH,  //Rock
        GraphicsLoader::GEO_ROCK_ROCK,  //Rock
        GraphicsLoader::GEO_ROCK_PILLAR,  //Rock
    };

    return list[num];

    //if (line == "bush")
    //    return GraphicsLoader::GEO_GRASS1;
    //if (line == "long bush")
    //    return GraphicsLoader::GEO_GRASS2;
    //if (line == "tree2")
    //    return GraphicsLoader::GEO_TREE1;
    //if (line == "tree4")
    //    return GraphicsLoader::GEO_TREE2;

    //return GraphicsLoader::NUM_GEOMETRY;
}

COMPONENTS ConvertStringToComponent(std::string line)
{
    std::string list[11] = {
        "displacement",
        "velocity",
        "appearance",
        "hitbox",
        "trap",
        "AI",
        "capture",
        "bait",
        "moneytree",
        "obstacle",
    };

    COMPONENTS component[11] = {
        COMPONENT_DISPLACEMENT,
        COMPONENT_VELOCITY,
        COMPONENT_APPEARANCE,
        COMPONENT_HITBOX,
        COMPONENT_TRAP,
        COMPONENT_AI,
        COMPONENT_CAPTURE,
        COMPONENT_BAIT,
        COMPONENT_MONEYTREE,
        COMPONENT_OBSTACLE,
    };

    for (int i = 0; i < 11; ++i)
    {
        if (line == list[i])
            return component[i];
    }
    
    return COMPONENT_NONE;
}


void LoadLevelMapData(std::ifstream& fileStream)
{
    if (Scene::m_levelMap)
    {
        for (int i = 0; i < Scene::m_rows; ++i)
        {
			delete Scene::m_levelMap[i];
        }
        delete Scene::m_levelMap;
    }

    Scene::m_levelMap = new char*[Scene::m_rows];  // rows
    for (int i = 0; i < Scene::m_rows; ++i)
    {
        Scene::m_levelMap[i] = new char[Scene::m_cols];
    }

    std::string line;
    int rowCount = 0;
    while (!fileStream.eof())
    {
        std::getline(fileStream, line);
        std::stringstream dataStream(line);
        std::string data;
        std::getline(dataStream, data, ',');
        if (line[0] == '#' || line == "")   // empty line OR comment
            continue;

        for (int i = 0; i < Scene::m_cols; ++i)
        {
            Scene::m_levelMap[rowCount][i] = data[0];
            std::getline(dataStream, data, ',');
        }

        ++rowCount;
    }
}

void LoadSaveData(std::ifstream& fileStream)
{
    std::string line;
    std::string data;

    // first content is name of the data - ignore
    // second content onwards is the data

    //==========================
    // FIRST LINE - QUEST COUNT
    //==========================
    std::getline(fileStream, line);
    std::stringstream dataStream(line);
    std::getline(dataStream, data, ',');
    std::getline(dataStream, data);
    int tempQuestCount = std::stoi(data);
    SharedData::GetInstance()->saveData->questCount = tempQuestCount;

    //====================================
    // SECOND LINE - QUEST ACTIVATED BOOL
    //====================================
    std::getline(fileStream, line);
    std::stringstream dataStream2(line);
    std::getline(dataStream2, data, ',');
    std::getline(dataStream2, data);
    bool tempQuestActivated = std::stoi(data);
    SharedData::GetInstance()->saveData->questActivated = tempQuestActivated;

    //=======================
    // THIRD LINE - CURRENCY
    //=======================
    std::getline(fileStream, line);
    std::stringstream dataStream3(line);
    std::getline(dataStream3, data, ',');
    std::getline(dataStream3, data);
    int tempCurrency = std::stoi(data);
    SharedData::GetInstance()->saveData->playerCurrency = tempCurrency;

    //==================================
    // 4th - 8th content: item quantity
    //==================================
    // Rock
    std::getline(fileStream, line);
    std::stringstream dataStreamI1(line);
    std::getline(dataStreamI1, data, ',');
    std::getline(dataStreamI1, data);
    int tempRockQuantity = std::stoi(data);
    SharedData::GetInstance()->saveData->rockQuantity = tempRockQuantity;

    // Net
    std::getline(fileStream, line);
    std::stringstream dataStreamI2(line);
    std::getline(dataStreamI2, data, ',');
    std::getline(dataStreamI2, data);
    int tempNetQuantity = std::stoi(data);
    SharedData::GetInstance()->saveData->netQuantity = tempNetQuantity;

    // Bait
    std::getline(fileStream, line);
    std::stringstream dataStreamI3(line);
    std::getline(dataStreamI3, data, ',');
    std::getline(dataStreamI3, data);
    int tempBaitQuantity = std::stoi(data);
    SharedData::GetInstance()->saveData->baitQuantity = tempBaitQuantity;

    // Trap
    std::getline(fileStream, line);
    std::stringstream dataStreamI4(line);
    std::getline(dataStreamI4, data, ',');
    std::getline(dataStreamI4, data);
    int tempTrapQuantity = std::stoi(data);
    SharedData::GetInstance()->saveData->trapQuantity = tempTrapQuantity;

    // Meat
    std::getline(fileStream, line);
    std::stringstream dataStreamI5(line);
    std::getline(dataStreamI5, data, ',');
    std::getline(dataStreamI5, data);
    int tempMeatQuantity = std::stoi(data);
    SharedData::GetInstance()->saveData->meatQuantity = tempMeatQuantity;

    //==========================
    // FOURTH LINE - ROCK LEVEL
    //==========================
    std::getline(fileStream, line);
    std::stringstream dataStream4(line);
    std::getline(dataStream4, data, ',');
    std::getline(dataStream4, data);
    int tempRockLevel = std::stoi(data);
    SharedData::GetInstance()->saveData->rockUpgradeLevel = tempRockLevel;

    //=========================
    // FIFTH LINE - NET LEVEL
    //=========================
    std::getline(fileStream, line);
    std::stringstream dataStream5(line);
    std::getline(dataStream5, data, ',');
    std::getline(dataStream5, data);
    int tempNetLevel = std::stoi(data);
    SharedData::GetInstance()->saveData->netUpgradeLevel = tempNetLevel;

    //==========================
    // SIXTH LINE - BAIT LEVEL
    //==========================
    std::getline(fileStream, line);
    std::stringstream dataStream6(line);
    std::getline(dataStream6, data, ',');
    std::getline(dataStream6, data);
    int tempBaitLevel = std::stoi(data);
    SharedData::GetInstance()->saveData->baitUpgradeLevel = tempBaitLevel;

    //===========================
    // SEVENTH LINE - TRAP LEVEL
    //===========================
    std::getline(fileStream, line);
    std::stringstream dataStream7(line);
    std::getline(dataStream7, data, ',');
    std::getline(dataStream7, data);
    int tempTrapLevel = std::stoi(data);
    SharedData::GetInstance()->saveData->trapUpgradeLevel = tempTrapLevel;

    //===================================
    // EIGHTH LINE - ZOO GRASS AREA SIZE
    //===================================
    std::getline(fileStream, line);
    std::stringstream dataStream8(line);
    std::getline(dataStream8, data, ',');
    std::getline(dataStream8, data);
    int tempGrassSize = std::stoi(data);
    SharedData::GetInstance()->saveData->grassZooSize = tempGrassSize;

    //==================================
    // NINTH LINE - ZOO SWAMP AREA SIZE
    //==================================
    std::getline(fileStream, line);
    std::stringstream dataStream9(line);
    std::getline(dataStream9, data, ',');
    std::getline(dataStream9, data);
    int tempSwampSize = std::stoi(data);
    SharedData::GetInstance()->saveData->swampZooSize = tempSwampSize;

    //=================================
    // TENTH LINE - ZOO ROCK AREA SIZE
    //=================================
    std::getline(fileStream, line);
    std::stringstream dataStream10(line);
    std::getline(dataStream10, data, ',');
    std::getline(dataStream10, data);
    int tempRockSize = std::stoi(data);
    SharedData::GetInstance()->saveData->rockZooSize = tempRockSize;

    //====================================
    // ELEVENTH LINE - ZOO LAVA AREA SIZE
    //====================================
    std::getline(fileStream, line);
    std::stringstream dataStream11(line);
    std::getline(dataStream11, data, ',');
    std::getline(dataStream11, data);
    int tempLavaSize = std::stoi(data);
    SharedData::GetInstance()->saveData->lavaZooSize = tempLavaSize;

    //================================
    // TWELFTH LINE - MONSTERS CAUGHT
    //================================
    std::getline(fileStream, line);
    std::stringstream dataStream12(line);
    std::getline(dataStream12, data, ',');
    std::getline(dataStream12, data, ',');
    for (; data != "end"; std::getline(dataStream12, data, ','))
    {
        SharedData::GetInstance()->saveData->monsterList.push_back(data);
    }
}