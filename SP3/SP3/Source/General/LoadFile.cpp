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

    case FILE_SAVEDATA:
        LoadSaveData(fileStream);
        break;
    }

    fileStream.close();
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

        std::vector<int> stats;
        for (int i = 0; i < 4; ++i)
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
        std::getline(dataStream, data, ',');

        // first content is name of tile - ignore

        // second content is tile count
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
                //std::cout << "Converted " << data << std::endl;
            }
            prevData = data;
        }

        // push into map
        Scene::AddToMap(tempTileCount, AssignMeshType(tempTileCount - 65), tempComponents);
    }
}

GraphicsLoader::GEOMETRY_TYPE AssignMeshType(int num)
{
    GraphicsLoader::GEOMETRY_TYPE list[GraphicsLoader::NUM_GEOMETRY] = {    // legend size
        GraphicsLoader::GEO_GRASS1,
        GraphicsLoader::GEO_GRASS2,
        GraphicsLoader::GEO_TREE1,
        GraphicsLoader::GEO_TREE2,
        GraphicsLoader::GEO_SWAMP_PLANT,
        GraphicsLoader::GEO_SWAMP_ROOT,
		GraphicsLoader::GEO_MONEY_TREE,
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

}