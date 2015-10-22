#pragma once


class CParameters;
class CSetPoint;

//AIRPLANE PITCH CONTROL
class CPitchControl: public CDynamicModel
{
	CSetPoint *m_pSetpoint;

public:
	CPitchControl(CParameters* pParameters);
	~CPitchControl();

	void reset(CState *s);
	void executeAction(CState *s,CAction *a,double dt);
};
