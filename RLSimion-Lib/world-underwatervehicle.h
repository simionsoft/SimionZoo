#pragma once

#include "world.h"

class CSetPoint;
class CRewardFunction;

class CUnderwaterVehicle : public CDynamicModel
{
	int m_sVSetpoint, m_sV, m_sVDeviation;
	int m_aUThrust;
	CSetPoint *m_pSetpoint;
public:

	CUnderwaterVehicle(CConfigNode* pParameters,const char* worldDefinition);
	virtual ~CUnderwaterVehicle();

	void reset(CState *s);
	void executeAction(CState *s, const CAction *a, double dt);
};
