#include "QuestManager.h"
#include "../LoadFile.h"
#include "../SharedData.h"

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
    if (m_questList.empty())
        return 0;
    
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

    // remove monsters
    int count = 0;
    for (std::vector<std::string>::iterator it = SharedData::GetInstance()->player->monsterList.begin(); it != SharedData::GetInstance()->player->monsterList.end(); )
    {
        if (*it == quest->GetRequiredMonster())
        {
            ++count;
            it = SharedData::GetInstance()->player->monsterList.erase(it);
        }
        else
        {
            ++it;
        }

        if (count >= quest->GetRequiredQuantity())
            break;
    }

    // reward player
    SharedData::GetInstance()->player->m_currency += 100 * quest->GetSerialNumber();
    delete quest;

    b_questActive = false;
}

bool QuestManager::IsQuestActive()
{
    return b_questActive;
}

bool QuestManager::IsCurrentQuestCompletable()
{
    if (GetCurrentQuest())  // there is a quest
    {
        int monsterCount = 0;
        for (unsigned i = 0; i < SharedData::GetInstance()->player->monsterList.size(); ++i)
        {
            if (GetCurrentQuest()->GetRequiredMonster() == SharedData::GetInstance()->player->monsterList[i])
            {
                ++monsterCount;
            }
        }

        if (monsterCount >= GetCurrentQuest()->GetRequiredQuantity())
            return true;
    }
    
    return false;
}

void QuestManager::SetQuestActive()
{
    b_questActive = true;
}

void QuestManager::SetQuestInactive()
{
    b_questActive = false;
}

void QuestManager::PopQuest()
{
    Quest* quest = m_questList.front();
    m_questList.pop();

    delete quest;
}