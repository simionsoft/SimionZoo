#pragma once

#include "world.h"
class CSetPoint;

//AIRPLANE PITCH CONTROL
class CPitchControl: public CDynamicModel
{
	int m_sSetpointPitch, m_sAttackAngle;
	int m_sPitch, m_sPitchRate, m_sControlDeviation;
	int m_aPitch;
	CSetPoint *m_pSetpoint;

public:
	CPitchControl(const char* worldDefinition, CParameters* pParameters);
	~CPitchControl();

	void reset(CState *s);
	void executeAction(CState *s,CAction *a,double dt);
};
