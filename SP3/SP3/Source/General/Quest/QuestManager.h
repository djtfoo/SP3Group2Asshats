#ifndef QUESTMANAGER_H
#define QUESTMANAGER_H

#include "Quest.h"
#include <queue>

class QuestManager
{
    std::queue<Quest*> m_questList;

    bool b_questActive;     // have to "accept" next quest

public:
    QuestManager();
    ~QuestManager();

    Quest* GetCurrentQuest();
    void AddToQueue(Quest* newQuest);
    void CompleteCurrentQuest();
};

#endif