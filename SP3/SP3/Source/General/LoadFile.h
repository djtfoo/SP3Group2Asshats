/******************************************************************************/
/*!
\file	LoadFile.h
\author Foo Jing Ting
\par	email: 152856H@mymail.nyp.edu.sg
\brief
Function to read CSV files for game data
*/
/******************************************************************************/
#ifndef LOAD_FILE_H
#define LOAD_FILE_H

#include <fstream>
#include <string>
#include "../Graphics/GraphicsLoader/GraphicsLoader.h"
#include "../GameObject/GameObject.h"

enum FILE_TYPE
{
    FILE_MONSTERDATA,
    FILE_LEVELGENERATIONDATA,
    FILE_LEVELMAPDATA,
    FILE_QUESTDATA,
    FILE_SAVEDATA,
    FILE_TOTAL
};


// array for monster data
// array for scene generation data
// array for every level design -- maybe don't need; load file when opening scene

bool LoadFile(const char* file_path, FILE_TYPE file_type);
    // file_path is: "GameData//filename.csv";

void LoadMonsterData(std::ifstream& fileStream);
void LoadLevelGenerationData(std::ifstream& fileStream);
void LoadLevelMapData(std::ifstream& fileStream);
void LoadQuestData(std::ifstream& fileStream);
void LoadSaveData(std::ifstream& fileStream);

GraphicsLoader::GEOMETRY_TYPE AssignMeshType(int num);
COMPONENTS ConvertStringToComponent(std::string line);

//void ConvertStringToEnum()
//{
//    std::string components[COMPONENT_MAX] =
//    {
//        "displacement",
//        "velocity",
//        "appearance",
//        "hitbox",
//    };
//
//    if (stringtoread == components[i])
//        return static_cast<COMPONENT>(i);
//}

#endif