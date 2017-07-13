#pragma once


#include "controller.h"
#include "vfa-critic.h"
#include "policy-learner.h"

class CExtendedWindTurbineVidalController: public CWindTurbineVidalController
{
	double m_td;
	CHILD_OBJECT_FACTORY<ICritic> m_pCritic;
	CHILD_OBJECT_FACTORY<CPolicyLearner> m_pAdpParamA, m_pAdpParamK_alpha, m_pAdpParamKP;

public:
	CExtendedWindTurbineVidalController(CConfigNode* pConfigNode);
	virtual ~CExtendedWindTurbineVidalController();

	virtual double selectAction(const CState *s, CAction *a);
	virtual double update(const CState *s, const CAction *a, const CState *s_p, double r, double behaviorProb);
};

class CExtendedWindTurbineBoukhezzarController : public CWindTurbineBoukhezzarController
{
	double m_td;
	CHILD_OBJECT_FACTORY<ICritic> m_pCritic;
	CHILD_OBJECT_FACTORY<CPolicyLearner> m_pAdpParamA, m_pAdpParamK_alpha, m_pAdpParamKP;

public:
	CExtendedWindTurbineBoukhezzarController(CConfigNode* pConfigNode);
	virtual ~CExtendedWindTurbineBoukhezzarController();

	virtual double selectAction(const CState *s, CAction *a);
	virtual double update(const CState *s, const CAction *a, const CState *s_p, double r, double behaviorProb) { return 1.0; }
};

class CExtendedWindTurbineJonkmanController : public CWindTurbineJonkmanController
{
	double m_td;
	CHILD_OBJECT_FACTORY<ICritic> m_pCritic;
	CHILD_OBJECT_FACTORY<CPolicyLearner> m_pAdpParamA, m_pAdpParamK_alpha, m_pAdpParamKP;

public:
	CExtendedWindTurbineJonkmanController(CConfigNode* pConfigNode);
	virtual ~CExtendedWindTurbineJonkmanController();

	virtual double selectAction(const CState *s, CAction *a);
	virtual double update(const CState *s, const CAction *a, const CState *s_p, double r, double behaviorProb) { return 1.0; }
};