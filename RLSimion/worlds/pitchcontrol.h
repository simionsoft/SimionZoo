#pragma once

#include "world.h"
class SetPoint;
class RewardFunction;

//AIRPLANE PITCH CONTROL
class PitchControl: public DynamicModel
{
	int m_sSetpointPitch, m_sAttackAngle;
	int m_sPitch, m_sPitchRate, m_sControlDeviation;
	int m_aPitch;
	SetPoint *m_pSetpoint;
public:
	PitchControl(ConfigNode* pParameters);
	virtual ~PitchControl();

	void reset(State *s);
	void executeAction(State *s, const Action *a, double dt);
};
