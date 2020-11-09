#pragma once
#if defined(__linux__) || defined(_WIN64)
#include "simion.h"
#include "parameters-numeric.h"
#include "deferred-load.h"

#include <vector>
using namespace std;

class IDeepNetwork;
class DeepDeterministicPolicy;
class IDeterministicPolicyNetwork;
class DeepVFunction;
class IVFunctionNetwork;
class Noise;
class DeepMinibatch;


class DeepCACLA : public Simion, DeferredLoad
{
	//Actor
	DeepMinibatch* m_pActorMinibatch= nullptr;
	IDeterministicPolicyNetwork* m_pActorNetwork= nullptr;
	CHILD_OBJECT<DeepDeterministicPolicy> m_actorPolicy;
	MULTI_VALUE_FACTORY<Noise> m_noiseSignals;
	//Critic
	DeepMinibatch* m_pCriticMinibatch= nullptr;
	IVFunctionNetwork* m_pCriticOnlineNetwork= nullptr;
	IVFunctionNetwork* m_pCriticTargetNetwork= nullptr;
	CHILD_OBJECT<DeepVFunction> m_criticVFunction;
	vector<double> m_V_s_p;
	vector<double> m_V_s;
public:
	DeepCACLA(ConfigNode* pConfigNode);
	virtual ~DeepCACLA();

	virtual double update(const State *s, const Action *a, const State *s_p, double r, double probability);

	virtual double selectAction(const State *s, Action *a);

	void deferredLoadStep();
};

#endif