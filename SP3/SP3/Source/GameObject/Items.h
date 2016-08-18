#ifndef ITEMS_H
#define ITEMS_H

#include <string>

static const int MAX_UPGRADE_LEVEL = 5;
static const int MAX_ITEM_TYPE_COUNT = 100;

class Item
{
public:
    enum TYPE
    {
        TYPE_NET,
        TYPE_BAIT,
        TYPE_MEAT,
        TYPE_TRAP_ONE,
        TYPE_TRAP_TWO,
        TYPE_ROCK,

        NUM_TYPE
    };

    int GetBuyCost();
    int GetUpgradeCost();
    int GetEffectiveness();
    int GetItemType();
    int GetCurrentUpgradeLevel();

    bool Use();
    bool Add(int addCount);
    bool Upgrade();

    Item();
    Item(std::string name, TYPE type, int buyCost, int upgradeCost, int effectiveness);
    ~Item();

protected:
    std::string m_name;
    
    TYPE m_type;
    int m_count;
    int m_buyCost;
    int m_upgradeCost;
    int m_effectiveness;
    unsigned m_currentUpgradeLevel;
};

#endif