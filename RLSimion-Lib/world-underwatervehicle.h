#pragma once

#include "world.h"

class CSetPoint;

class CUnderwaterVehicle : public CDynamicModel
{
	CSetPoint *m_pSetpoint;
public:

	CUnderwaterVehicle(tinyxml2::XMLElement* pParameters);
	~CUnderwaterVehicle();

	void reset(CState *s);
	void executeAction(CState *s,CAction *a,double dt);
};
