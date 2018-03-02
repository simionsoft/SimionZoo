#pragma once

#include "world.h"
class CSetPoint;
#include "../reward.h"
#include "../parameters.h"

//Implementation according to:
//https://homes.cs.washington.edu/~todorov/courses/amath579/reading/Continuous.pdf
//as used in "Model-Free Reinforcement Learning with Continous Action in Practice"

//SWINGUP PENDULUM
class CSwingupPendulum : public CDynamicModel
{
	int m_sAngularVelocity, m_sAngle;
	int m_sNormalizedTimeInTargetPosition;
	int m_aTorque;

	//DOUBLE_PARAM 

public:
	CSwingupPendulum(CConfigNode* pParameters);
	virtual ~CSwingupPendulum();

	void reset(CState *s);

	void executeAction(CState *s, const CAction *a, double dt);
};

class CSwingupPendulumReward : public IRewardComponent
{
public:
	CSwingupPendulumReward() = default;
	double getReward(const CState *s, const CAction *a, const CState *s_p);
	const char* getName() { return "reward"; }
	double getMin();
	double getMax();
};