/******************************************************************************/
/*!
\file	Monster.h
\author Foo Jing Ting
\par	email: 152856H@mymail.nyp.edu.sg
\brief
Class that defines a monster's variables and statistics
*/
/******************************************************************************/
#ifndef MONSTER_H
#define MONSTER_H

#include <string>
#include "Vector3.h"
#include "Pathfinding.h"
//#include "Player.h"
//#include "AI_Strategy.h"

//class AI_Strategy;

class Monster
{
	friend class AI_Strategy;

public:
    virtual ~Monster();
    
    std::string GetName();
    int GetHealthStat();
    float GetCaptureRateStat();
    float GetAggressionStat();
    float GetFearStat();
    bool CheckCapture();

    Vector3 m_position;
    Vector3 m_destination;
    Vector3 m_velocity;
    Vector3 m_scale;  // for rendering

	void ResetAggression();
	void ResetFear();

    AI_Strategy* m_strategy;

    int GetStrategyState();
	virtual void Update(double dt) = 0;
    virtual void TakeDamage(const int damage) = 0;
    virtual void GetTrapped();
    virtual void GetCaptured();
    void SetIdleState();
    void AttackPlayer();

protected:
    Monster() {}
    Monster(std::string name, const std::vector<int>& stats);

    std::string m_name;

    // amount to increase stat by
    float AggressionLevel;
    float FearLevel;

    int m_healthStat;
    float m_captureRateStat;
    float m_aggressionStat;
    float m_fearStat;

	float m_originalAggression;
	float m_originalFear;

    Pathfinding* m_pathfinder;

    void move();
    void changeHealthStat(const int newHealth);
    void changeAggressionStat(const float newAggression);
    void changeFearStat(const float newFear);
    void changeCaptureRateStat(const float newCaptureRate);
};

#endif