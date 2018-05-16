#pragma once

#include "world.h"
class SetPoint;
class RewardFunction;

//AIRPLANE PITCH CONTROL
class PitchControl: public DynamicModel
{
	size_t m_sSetpointPitch, m_sAttackAngle;
	size_t m_sPitch, m_sPitchRate, m_sControlDeviation;
	size_t m_aPitch;
	SetPoint *m_pSetpoint;
public:
	PitchControl(ConfigNode* pParameters);
	virtual ~PitchControl();

	void reset(State *s);
	void executeAction(State *s, const Action *a, double dt);
};
