#pragma once

#include "world.h"
class SetPoint;
#include "../reward.h"

//CONTINUOUS GRIDWORLD
class ContinuousGridWorld : public DynamicModel
{
	size_t m_sXPosition, m_sYPosition;

	INT_PARAM m_targetX, m_targetY;

	//for continuous case
	size_t m_aXStep, m_aYStep;

public:
	ContinuousGridWorld(ConfigNode* pParameters);
	virtual ~ContinuousGridWorld();

	void reset(State *s);

	void executeAction(State *s, const Action *a, double dt);
};

class ContinuousGridWorldReward : public IRewardComponent
{
protected:
	double m_xTarget, m_yTarget;

public:
	ContinuousGridWorldReward(int targetX, int targetY);
	double getReward(const State *s, const Action *a, const State *s_p);
	const char* getName() { return "reward"; }
	double getMin();
	double getMax();
};

