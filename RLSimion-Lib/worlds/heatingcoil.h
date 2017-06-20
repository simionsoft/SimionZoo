#pragma once


class CParameters;
class CSetPoint;

//HEATING COIL
class CHeatingCoil: public CDynamicModel
{
	CSetPoint *m_pSetpoint;

public:
	CHeatingCoil(CParameters* pParameters);
	~CHeatingCoil();

	void reset(CState *s);
	void executeAction(CState *s,CAction *a,double dt);
};
