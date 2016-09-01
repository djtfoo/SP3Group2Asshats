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
    unsigned rockUpgradeLevel;
    unsigned netUpgradeLevel;
    unsigned baitUpgradeLevel;
    unsigned trapUpgradeLevel;
    std::vector<std::string> monsterList;

    SaveData();
    ~SaveData();

    void Init(const char* file_path);

    bool SaveGame();    //main function to be called by other classes
    void LoadGame();    //to load game data

private:
    void saveGameData();    //function to save the game's data to this struct
    bool writeToSaveFile(const char* file_path);    //function to write to the save data file
};

#endif