#pragma once

#include "world.h"
class SetPoint;
#include "../reward.h"
#include "../parameters.h"

//Implementation according to:
//https://scholarworks.umass.edu/cgi/viewcontent.cgi?article=1130&context=cs_faculty_pubs

//DOUBLE PENDULUM
class DoublePendulum : public DynamicModel
{
	size_t m_sTheta1, m_sTheta2;
	size_t m_sTheta1Dot, m_sTheta2Dot;

	size_t m_aTorque1, m_aTorque2;

	//DOUBLE_PARAM 

public:
	DoublePendulum(ConfigNode* pParameters);
	virtual ~DoublePendulum();

	void reset(State *s);

	void executeAction(State *s, const Action *a, double dt);
};

class DoublePendulumReward : public IRewardComponent
{
	double m_timeInGoal= 0.0;
public:
	DoublePendulumReward() = default;
	double getReward(const State *s, const Action *a, const State *s_p);
	const char* getName() { return "reward"; }
	double getMin();
	double getMax();
};