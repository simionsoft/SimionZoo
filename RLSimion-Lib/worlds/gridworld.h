#pragma once

#include "world.h"
class CSetPoint;
#include "../reward.h"

//CONTINUOUS GRIDWORLD
class CContinuousGridWorld : public CDynamicModel
{
	int m_sXPosition, m_sYPosition;

	INT_PARAM m_targetX, m_targetY;

	//for continuous case
	int m_aXStep, m_aYStep;

public:
	CContinuousGridWorld(CConfigNode* pParameters);
	virtual ~CContinuousGridWorld();

	void reset(CState *s);

	void executeAction(CState *s, const CAction *a, double dt);
};

class CContinuousGridWorldReward : public IRewardComponent
{
protected:
	double m_xTarget, m_yTarget;

public:
	CContinuousGridWorldReward(int targetX, int targetY);
	double getReward(const CState *s, const CAction *a, const CState *s_p);
	const char* getName() { return "reward"; }
	double getMin();
	double getMax();
};


//DISCRETE GRIDWORLD
class CDiscreteGridWorld : public CDynamicModel
{
	int m_sXPosition, m_sYPosition;

	INT_PARAM m_targetX, m_targetY;

	//for discrete case
	int m_aStep;

public:
	CDiscreteGridWorld(CConfigNode* pParameters);
	virtual ~CDiscreteGridWorld();

	void reset(CState *s);

	void executeAction(CState *s, const CAction *a, double dt);
};

class CDiscreteGridWorldReward : public IRewardComponent
{
protected:
	int m_xTarget, m_yTarget;

public:
	CDiscreteGridWorldReward(int targetX, int targetY);
	double getReward(const CState *s, const CAction *a, const CState *s_p);
	const char* getName() { return "reward"; }
	double getMin();
	double getMax();
};