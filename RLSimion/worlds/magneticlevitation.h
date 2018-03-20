#pragma once


#include "world.h"
class SetPoint;

//MAGNETIC LEVIATION OF A STEEL BALL
class MagneticLevitation: public DynamicModel
{
	int m_sPosition, m_sVelocity, m_sCurrent;
	int m_sPositionSetpoint, m_sPositionDeviation;
	int m_aVoltage;
	SetPoint *m_pEvalSetPoint,*m_pLearnSetPoint;

public:
	MagneticLevitation(ConfigNode* pParameters);
	virtual ~MagneticLevitation();

	void reset(State *s);
	void executeAction(State *s,Action *a,double dt);
};
