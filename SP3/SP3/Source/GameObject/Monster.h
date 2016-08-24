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
    int GetCaptureRateStat();
    int GetAggressionStat();
    int GetFearStat();
    bool CheckCapture();
	void SetPosition(Vector3 m_position);
	Vector3 GetPosition();

	void ResetAggression();
	void ResetFear();

	virtual void Update(double dt) = 0;

protected:
    Monster() {}
    Monster(std::string name, int stats[]);

    std::string m_name;

    int m_healthStat;
    int m_captureRateStat;
    int m_aggressionStat;
    int m_fearStat;

	int m_originalAggression;
	int m_originalFear;

	AI_Strategy* m_strategy;

    Pathfinding* m_pathfinder;

    Vector3 m_position;
    Vector3 m_velocity;
    Vector3 m_scale;  // for rendering

    void move();
    void changeHealthStat(const int newHealth);
    void changeAggressionStat(const int newAggression);
    void changeFearStat(const int newFear);
    void changeCaptureRateStat(const int newCaptureRate);
};

#endif