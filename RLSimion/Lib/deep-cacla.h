#pragma once

#include "simion.h"
#include "parameters.h"
#include "parameters-numeric.h"
#include "deep-functions.h"
#include "deferred-load.h"

#include <vector>
using namespace std;

class IDeepNetwork;

class DeepCACLA : public Simion, DeferredLoad
{
	//Actor
	DeepMinibatch* m_pActorMinibatch;
	IDeepNetwork* m_pActorNetwork;
	CHILD_OBJECT<DeepDeterministicPolicy> m_actorPolicy;
	//Critic
	DeepMinibatch* m_pCriticMinibatch;
	IDeepNetwork* m_pCriticNetwork;
	CHILD_OBJECT<DeepVFunction> m_criticVFunction;
	vector<double> m_V_s;
	vector<double> m_V_s_p;
	
public:
	DeepCACLA(ConfigNode* pConfigNode);
	virtual ~DeepCACLA();

	virtual double update(const State *s, const Action *a, const State *s_p, double r, double probability);

	virtual double selectAction(const State *s, Action *a);

	void deferredLoadStep();
};

