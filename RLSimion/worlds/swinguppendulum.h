#pragma once

#include "world.h"
class SetPoint;
#include "../reward.h"
#include "../parameters.h"

//Implementation according to:
//https://homes.cs.washington.edu/~todorov/courses/amath579/reading/Continuous.pdf
//as used in "Model-Free Reinforcement Learning with Continous Action in Practice"

//SWINGUP PENDULUM
class SwingupPendulum : public DynamicModel
{
	size_t m_sAngularVelocity, m_sAngle;
	size_t m_aTorque;

	//DOUBLE_PARAM 

public:
	SwingupPendulum(ConfigNode* pParameters);
	virtual ~SwingupPendulum();

	void reset(State *s);

	void executeAction(State *s, const Action *a, double dt);
};

class SwingupPendulumReward : public IRewardComponent
{
	double m_timeInGoal= 0.0;
public:
	SwingupPendulumReward() = default;
	double getReward(const State *s, const Action *a, const State *s_p);
	const char* getName() { return "reward"; }
	double getMin();
	double getMax();
};