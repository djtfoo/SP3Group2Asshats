#include "QuestManager.h"

QuestManager::QuestManager()
{
}

QuestManager::~QuestManager()
{
}

Quest* QuestManager::GetCurrentQuest()
{
    return m_questList.front();
}

void QuestManager::AddToQueue(Quest* newQuest)
{
    m_questList.push(newQuest);
}

void QuestManager::CompleteCurrentQuest()
{
    Quest* quest = m_questList.front();
    m_questList.pop();
    delete quest;
}