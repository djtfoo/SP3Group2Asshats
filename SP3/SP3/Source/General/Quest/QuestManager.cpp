#include "QuestManager.h"
#include "../LoadFile.h"

QuestManager::QuestManager()
{
    b_questActive = false;
}

QuestManager::~QuestManager()
{
}

void QuestManager::Init()
{
    LoadFile("GameData//QuestData.csv", FILE_QUESTDATA);
}

void QuestManager::Exit()
{
    while (m_questList.size() > 0)
    {
        Quest *quest = m_questList.front();
        delete quest;
        m_questList.pop();
    }
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