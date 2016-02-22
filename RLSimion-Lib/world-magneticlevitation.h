#pragma once


#include "world.h"
class CSetPoint;

//MAGNETIC LEVIATION OF A STEEL BALL
class CMagneticLevitation: public CDynamicModel
{
	int m_sPosition, m_sVelocity, m_sCurrent;
	int m_sPositionSetpoint, m_sPositionDeviation;
	int m_aVoltage;
	CSetPoint *m_pEvalSetPoint,*m_pLearnSetPoint;

public:
	CMagneticLevitation(tinyxml2::XMLElement* pParameters);
	~CMagneticLevitation();

	void reset(CState *s);
	void executeAction(CState *s,CAction *a,double dt);
};
