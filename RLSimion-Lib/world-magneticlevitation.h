#pragma once


#include "world.h"
class CSetPoint;

//MAGNETIC LEVIATION OF A STEEL BALL
class CMagneticLevitation: public CDynamicModel
{
	CSetPoint *m_pEvalSetPoint,*m_pLearnSetPoint;

public:
	CMagneticLevitation(tinyxml2::XMLElement* pParameters);
	~CMagneticLevitation();

	void reset(CState *s);
	void executeAction(CState *s,CAction *a,double dt);
};
