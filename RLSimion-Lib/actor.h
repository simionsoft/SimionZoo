#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

#include "parameters.h"
#include "delayed-load.h"
#include "policy-learner.h"
#include "controller.h"
#include <vector>

class CActor: public CDeferredLoad
{
	CHILD_OBJECT_FACTORY<CController> m_pInitController;
protected:
	MULTI_VALUE_FACTORY<CPolicyLearner> m_policyLearners;

public:
	CActor(CConfigNode* pConfigNode);
	CActor() = default;
	virtual ~CActor();

	//this methods sets its output pi(s) in a
	virtual double selectAction(const CState *s, CAction *a);

	virtual void update(const CState *s, const CAction *a, const CState *s_p, double r, double td);

	//Returns the probability of outputting action a in state s
	//bStochastic= true, if we want to consider exploration
	//bStochastic= false, if we want to only consider the greedy policy
	virtual double getActionProbability(const CState *s, const CAction *a, bool bStochastic);

	//CDeferredLoad
	virtual void deferredLoadStep();
};
