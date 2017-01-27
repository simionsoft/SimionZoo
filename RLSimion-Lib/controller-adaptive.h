#pragma once


#include "controller.h"
#include "vfa-critic.h"
#include "policy-learner.h"

class CExtendedWindTurbineVidalController: public CWindTurbineVidalController
{
	double m_td;
	CHILD_OBJECT_FACTORY<CCritic> m_pCritic;
	CHILD_OBJECT_FACTORY<CPolicyLearner> m_pAdpParamA, m_pAdpParamK_alpha, m_pAdpParamKP;

public:
	CExtendedWindTurbineVidalController(CConfigNode* pConfigNode);
	virtual ~CExtendedWindTurbineVidalController();

	void selectAction(const CState *s, CAction *a);
	virtual void updateValue(const CState *s, const CAction *a, const CState *s_p, double r);
	virtual void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r);
};