#include "Quest.h"

Quest::Quest(unsigned int serialNum, std::string reqMonster, unsigned int reqQUantity) : m_serialNumber(serialNum), m_requiredMonster(reqMonster), m_requiredQuantity(reqQUantity)
{
}

Quest::~Quest()
{
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