#pragma once

#include "world.h"

class CParameters;
class CSetPoint;

class CUnderwaterVehicle : public CDynamicModel
{
	CSetPoint *m_pSetpoint;
public:

	CUnderwaterVehicle(CParameters* pParameters);
	~CUnderwaterVehicle();

	void reset(CState *s);
	void executeAction(CState *s,CAction *a,double dt);
};
