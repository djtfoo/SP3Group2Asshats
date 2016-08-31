#ifndef QUEST_H
#define QUEST_H

#include <string>

class Quest
{
    unsigned int m_serialNumber;
    std::string m_requiredMonster;
    unsigned int m_requiredQuantity;

public:
    Quest(unsigned int serialNum, std::string reqMonster, unsigned int reqQUantity);
    ~Quest();

    unsigned int GetSerialNumber();
    std::string GetRequiredMonster();
    unsigned int GetRequiredQuantity();
};

#endif