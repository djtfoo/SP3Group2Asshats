#include "Items.h"
#include "../General/SharedData.h"

Item::Item() : 
m_type(NUM_TYPE),
m_buyCost(0),
m_upgradeCost(0),
m_effectiveness(0),
m_count(0),
m_currentUpgradeLevel(0)
{
}

Item::Item(std::string name, TYPE type, int buyCost, int upgradeCost, int effectiveness) :
m_count(0),
m_currentUpgradeLevel(0)
{
    this->m_name = name;
    this->m_type = type;
    this->m_buyCost = buyCost;
    this->m_upgradeCost = upgradeCost;
    this->m_effectiveness = effectiveness;
}

Item::~Item()
{

}
bool Item::Upgrade()
{
    if (this->m_currentUpgradeLevel >= MAX_UPGRADE_LEVEL)
        return false;
    else
        switch (this->m_type)
        {
            case TYPE_NET:
                this->m_effectiveness += 5;
                this->m_currentUpgradeLevel++;
                return true;
            case TYPE_BAIT:
                this->m_effectiveness += 5;
                this->m_currentUpgradeLevel++;
                return true;
            case TYPE_MEAT:
                //You cannot upgrade meat
                return false;
            case TYPE_TRAP:
                this->m_effectiveness += 200;
                this->m_currentUpgradeLevel++;
                return true;
            case TYPE_ROCK:
                this->m_effectiveness += 5;
                this->m_currentUpgradeLevel++;
                return true;

            default:
                return false;
        }
}

int Item::GetBuyCost()
{
    return this->m_buyCost;
}

int Item::GetUpgradeCost()
{
    return this->m_upgradeCost;
}

int Item::GetEffectiveness()
{
    return this->m_effectiveness;
}

int Item::GetItemType()
{
    return this->m_type;
}

int Item::GetCurrentUpgradeLevel()
{
    return this->m_currentUpgradeLevel;
}

bool Item::Use()
{
    if (m_count > 0)
    {
        this->m_count--;
        return true;
    }

    return false;
}

bool Item::Add(int addCount)
{
    if (this->m_count + addCount <= MAX_ITEM_TYPE_COUNT)
    {
        this->m_count += addCount;
        return true;
    }

    return false;
}

int Item::GetCount()
{
    return this->m_count;
}

bool Item::Buy(int count)
{
    if (SharedData::GetInstance()->player->m_currency >= count * this->m_buyCost)
    {
        this->m_count += count;
        SharedData::GetInstance()->player->m_currency -= count * this->m_buyCost;
        return true;
    }
    
    return false;
}

bool Item::Sell(int count)
{
    if (this->m_count >= count)
    {
        this->m_count -= count;
        SharedData::GetInstance()->player->m_currency += count * this->m_buyCost * 0.5;
        return true;
    }
    
    return false;
}