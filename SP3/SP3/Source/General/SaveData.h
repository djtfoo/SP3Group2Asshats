#ifndef SAVEDATA_H
#define SAVEDATA_H

#include <string>
#include <vector>

struct SaveData
{
    bool b_saveGameExists;

    unsigned int questCount;    // current quest player is at
    bool questActivated; // whether player started the quest
    int playerCurrency;
    
    int rockQuantity;
    int netQuantity;
    int baitQuantity;
    int trapQuantity;
    int meatQuantity;

    unsigned rockUpgradeLevel;
    unsigned netUpgradeLevel;
    unsigned baitUpgradeLevel;
    unsigned trapUpgradeLevel;
    
    int grassZooSize;
    int swampZooSize;
    int rockZooSize;
    int lavaZooSize;

    std::vector<std::string> monsterList;

    SaveData();
    ~SaveData();

    void Init(const char* file_path);

    void ClearGame();   //clear all game data for starting new game, or before loading game
    bool SaveGame();    //main function to be called by other classes
    void LoadGame();    //to load game data

private:
    void saveGameData();    //function to save the game's data to this struct
    bool writeToSaveFile(const char* file_path);    //function to write to the save data file
};

#endif