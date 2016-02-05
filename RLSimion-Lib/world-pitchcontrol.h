#pragma once

#include "world.h"
class CSetPoint;

//AIRPLANE PITCH CONTROL
class CPitchControl: public CDynamicModel
{
	CSetPoint *m_pSetpoint;

public:
	CPitchControl(tinyxml2::XMLElement* pParameters);
	~CPitchControl();

	void reset(CState *s);
	void executeAction(CState *s,CAction *a,double dt);
};
