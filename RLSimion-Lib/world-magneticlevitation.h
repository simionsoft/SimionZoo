#pragma once


class CParameters;
class CSetPoint;

//MAGNETIC LEVIATION OF A STEEL BALL
class CMagneticLevitation: public CDynamicModel
{
	CSetPoint *m_pEvalSetPoint,*m_pLearnSetPoint;

public:
	CMagneticLevitation(CParameters* pParameters);
	~CMagneticLevitation();

	void reset(CState *s);
	void executeAction(CState *s,CAction *a,double dt);
};
