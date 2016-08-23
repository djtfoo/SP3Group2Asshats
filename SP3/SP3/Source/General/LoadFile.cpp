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

    //unsigned int allocateCount = 0;
    //while (!fileStream.eof())
    //{
    //    std::getline(fileStream, line);
    //
    //    std::stringstream dataStream(line);
    //    std::string data;
    //    std::getline(dataStream, data, ',');
    //
    //    // first content is name
    //    string tempName = data;
    //
    //    // second content is elixir cost
    //    std::getline(dataStream, data, ',');
    //    int tempElixirCost = std::stoi(data);
    //
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

        int stats[4] = {};
        for (int i = 0; i < 4; ++i)
        {
            std::getline(dataStream, data, ',');
            stats[i] = std::stoi(data);
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
        int tempTileCount = std::stoi(data);

        if (tempTileCount == 0)
            continue;

        // remainder is components
        std::vector<COMPONENTS> tempComponents;
        std::getline(dataStream, data, ',');
        for (; data != ""; std::getline(dataStream, data, ','))
        {
            tempComponents.push_back(ConvertStringToComponent(data));
            std::cout << "Converted! ";
        }

        // push into map
        //MonsterFactory::AddToMap(tempName, stats);
        //std::map<int, std::pair<GraphicsLoader::GEOMETRY_TYPE, std::vector<COMPONENTS>> > m_levelGenerationData;
    }
}

GraphicsLoader::GEOMETRY_TYPE AssignMeshType(int num)
{
    GraphicsLoader::GEOMETRY_TYPE list[GraphicsLoader::NUM_GEOMETRY] = {    // legend size
        GraphicsLoader::GEO_GRASS1,
        GraphicsLoader::GEO_GRASS2,
        GraphicsLoader::GEO_TREE1,
        GraphicsLoader::GEO_TREE2,
    };

    return list[num - 1];

    //if (line == "bush")
    //    return GraphicsLoader::GEO_GRASS1;
    //if (line == "long bush")
    //    return GraphicsLoader::GEO_GRASS2;
    //if (line == "tree2")
    //    return GraphicsLoader::GEO_TREE1;
    //if (line == "tree4")
    //    return GraphicsLoader::GEO_TREE2;

    return GraphicsLoader::NUM_GEOMETRY;
}

COMPONENTS ConvertStringToComponent(std::string line)
{
    std::string list[10] = {
        "displacement",
        "velocity",
    };

    return COMPONENT_DISPLACEMENT;
}


void LoadLevelMapData(std::ifstream& fileStream)
{
    if (Scene::m_levelMap)
    {
        for (int i = 0; i < 50; ++i)
        {
            for (int j = 0; i < 50; ++j)
            {
                delete Scene::m_levelMap[i];
            }
        }
        delete Scene::m_levelMap;
    }

    Scene::m_levelMap = new char*[50];  // rows
    for (int i = 0; i < 50; ++i)
    {
        Scene::m_levelMap[i] = new char[50];
    }

    for (int i = 0; i < 50; ++i) {
        fileStream.read((char*)&Scene::m_levelMap[i][0], 50);
    }
    fileStream.close();
}

void LoadSaveData(std::ifstream& fileStream) 
{

}