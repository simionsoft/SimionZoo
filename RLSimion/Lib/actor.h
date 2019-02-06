#pragma once

class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;

#include "parameters.h"
#include "deferred-load.h"
#include "policy-learner.h"
#include "controller.h"
#include <vector>

class Actor: public DeferredLoad
{
	CHILD_OBJECT_FACTORY<Controller> m_pInitController;
protected:
	MULTI_VALUE_FACTORY<PolicyLearner> m_policyLearners;

public:
	Actor(ConfigNode* pConfigNode);
	Actor() = default;
	virtual ~Actor();

	//this methods sets its output pi(s) in a
	virtual double selectAction(const State *s, Action *a);

	virtual void update(const State *s, const Action *a, const State *s_p, double r, double td);

	//Returns the probability of outputting action a in state s
	//bStochastic= true, if we want to consider exploration
	//bStochastic= false, if we want to only consider the greedy policy
	virtual double getActionProbability(const State *s, const Action *a, bool bStochastic);

	//DeferredLoad
	virtual void deferredLoadStep();
};
