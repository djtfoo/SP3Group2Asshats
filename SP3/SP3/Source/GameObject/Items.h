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
        TYPE_TRAP,
        TYPE_ROCK,

        NUM_TYPE
    };

    int GetBuyCost();
    int GetUpgradeCost();
    int GetEffectiveness();
    int GetItemType();
    int GetCurrentUpgradeLevel();
    int GetCount();

    bool Use();
    bool Add(const int addCount);
    bool Upgrade();
    bool Buy(const int count);
    bool Sell(const int count);

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