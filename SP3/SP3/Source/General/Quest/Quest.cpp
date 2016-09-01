#include "Quest.h"

Quest::Quest(unsigned int serialNum, std::string questName, std::string reqMonster, unsigned int reqQuantity, std::string zone)
: m_serialNumber(serialNum)
, m_questName(questName)
, m_requiredMonster(reqMonster)
, m_requiredQuantity(reqQuantity)
, m_zone(zone)
{
}

Quest::~Quest()
{
}

std::string Quest::GetQuestName()
{
    return m_questName;
}

unsigned int Quest::GetSerialNumber()
{
    return m_serialNumber;
}

std::string Quest::GetRequiredMonster()
{
    return m_requiredMonster;
}

unsigned int Quest::GetRequiredQuantity()
{
    return m_requiredQuantity;
}

std::string Quest::GetZone()
{
    return m_zone;
}