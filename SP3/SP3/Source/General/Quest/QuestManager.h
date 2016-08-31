#ifndef QUESTMANAGER_H
#define QUESTMANAGER_H

#include "Quest.h"
#include <queue>

class QuestManager
{
    std::queue<Quest*> m_questList;

public:
    QuestManager();
    ~QuestManager();

    Quest* GetCurrentQuest();
    void AddToQueue(Quest* newQuest);
    void CompleteCurrentQuest();
};

#endif