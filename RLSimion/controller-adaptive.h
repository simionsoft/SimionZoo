#pragma once


#include "controller.h"
#include "vfa-critic.h"
#include "policy-learner.h"

class ExtendedWindTurbineVidalController: public WindTurbineVidalController
{
	size_t m_aActionId, m_KAlphaActionId, m_KPActionId;

	CHILD_OBJECT_FACTORY<ICritic> m_pCritic;
	CHILD_OBJECT_FACTORY<PolicyLearner> m_pALearner, m_pKAlphaLearner, m_pKPLearner;

public:
	ExtendedWindTurbineVidalController(ConfigNode* pConfigNode);
	virtual ~ExtendedWindTurbineVidalController();

	virtual double selectAction(const State *s, Action *a);
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb);
};

class ExtendedWindTurbineBoukhezzarController : public WindTurbineBoukhezzarController
{
	size_t m_C0ActionId, m_KPActionId;
	CHILD_OBJECT_FACTORY<ICritic> m_pCritic;
	CHILD_OBJECT_FACTORY<PolicyLearner> m_pC0Learner, m_pKPLearner;

public:
	ExtendedWindTurbineBoukhezzarController(ConfigNode* pConfigNode);
	virtual ~ExtendedWindTurbineBoukhezzarController();

	virtual double selectAction(const State *s, Action *a);
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb);
};

class ExtendedWindTurbineJonkmanController : public WindTurbineJonkmanController
{
	double m_td;
	size_t m_KPActionId;
	CHILD_OBJECT_FACTORY<ICritic> m_pCritic;
	CHILD_OBJECT_FACTORY<PolicyLearner> m_pPC_KP_Learner;

public:
	ExtendedWindTurbineJonkmanController(ConfigNode* pConfigNode);
	virtual ~ExtendedWindTurbineJonkmanController();

	virtual double selectAction(const State *s, Action *a);
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb);
};