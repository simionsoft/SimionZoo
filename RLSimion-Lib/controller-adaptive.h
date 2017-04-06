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

	double selectAction(const CState *s, CAction *a);
	virtual void update(const CState *s, const CAction *a, const CState *s_p, double r);
};

class CExtendedWindTurbineBoukhezzarController : public CWindTurbineBoukhezzarController
{
	double m_td;
	CHILD_OBJECT_FACTORY<CCritic> m_pCritic;
	CHILD_OBJECT_FACTORY<CPolicyLearner> m_pAdpParamA, m_pAdpParamK_alpha, m_pAdpParamKP;

public:
	CExtendedWindTurbineBoukhezzarController(CConfigNode* pConfigNode);
	virtual ~CExtendedWindTurbineBoukhezzarController();

	double selectAction(const CState *s, CAction *a);
	virtual void update(const CState *s, const CAction *a, const CState *s_p, double r);
};

class CExtendedWindTurbineJonkmanController : public CWindTurbineJonkmanController
{
	double m_td;
	CHILD_OBJECT_FACTORY<CCritic> m_pCritic;
	CHILD_OBJECT_FACTORY<CPolicyLearner> m_pAdpParamA, m_pAdpParamK_alpha, m_pAdpParamKP;

public:
	CExtendedWindTurbineJonkmanController(CConfigNode* pConfigNode);
	virtual ~CExtendedWindTurbineJonkmanController();

	double selectAction(const CState *s, CAction *a);
	virtual void update(const CState *s, const CAction *a, const CState *s_p, double r);
};