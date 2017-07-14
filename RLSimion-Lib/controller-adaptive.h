#pragma once


#include "controller.h"
#include "vfa-critic.h"
#include "policy-learner.h"

class CExtendedWindTurbineVidalController: public CWindTurbineVidalController
{
	unsigned int m_aActionId, m_KAlphaActionId, m_KPActionId;

	CHILD_OBJECT_FACTORY<ICritic> m_pCritic;
	CHILD_OBJECT_FACTORY<CPolicyLearner> m_pALearner, m_pKAlphaLearner, m_pKPLearner;

public:
	CExtendedWindTurbineVidalController(CConfigNode* pConfigNode);
	virtual ~CExtendedWindTurbineVidalController();

	virtual double selectAction(const CState *s, CAction *a);
	virtual double update(const CState *s, const CAction *a, const CState *s_p, double r, double behaviorProb);
};

class CExtendedWindTurbineBoukhezzarController : public CWindTurbineBoukhezzarController
{
	unsigned int m_C0ActionId, m_KPActionId;
	CHILD_OBJECT_FACTORY<ICritic> m_pCritic;
	CHILD_OBJECT_FACTORY<CPolicyLearner> m_pC0Learner, m_pKPLearner;

public:
	CExtendedWindTurbineBoukhezzarController(CConfigNode* pConfigNode);
	virtual ~CExtendedWindTurbineBoukhezzarController();

	virtual double selectAction(const CState *s, CAction *a);
	virtual double update(const CState *s, const CAction *a, const CState *s_p, double r, double behaviorProb);
};

class CExtendedWindTurbineJonkmanController : public CWindTurbineJonkmanController
{
	double m_td;
	CHILD_OBJECT_FACTORY<ICritic> m_pCritic;
	CHILD_OBJECT_FACTORY<CPolicyLearner> m_pALearner, m_pKAlphaLearner, m_pKPLearner;

public:
	CExtendedWindTurbineJonkmanController(CConfigNode* pConfigNode);
	virtual ~CExtendedWindTurbineJonkmanController();

	virtual double selectAction(const CState *s, CAction *a);
	virtual double update(const CState *s, const CAction *a, const CState *s_p, double r, double behaviorProb) { return 1.0; }
};