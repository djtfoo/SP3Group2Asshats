#ifndef QUEST_H
#define QUEST_H

#include <string>

class Quest
{
    unsigned int m_serialNumber;    //quest serial number (quests are given and cleared in this order)
    std::string m_questName;        //name of quest
    std::string m_requiredMonster;      //required monster for the quest
    unsigned int m_requiredQuantity;    //required quantity of that monster to complete the quest
    std::string m_zone; //zone the quest is at

public:
    Quest(unsigned int serialNum, std::string questName, std::string reqMonster, unsigned int reqQuantity, std::string zone);
    ~Quest();

    unsigned int GetSerialNumber();
    std::string GetQuestName();
    std::string GetRequiredMonster();
    unsigned int GetRequiredQuantity();
    std::string GetZone();
};

#endif