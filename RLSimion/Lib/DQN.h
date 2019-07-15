#pragma once
#if defined(__linux__) || defined(_WIN64)

#include <vector>
#include "simion.h"
#include "parameters.h"
#include "deferred-load.h"
#include "deep-functions.h"

class INetwork;
class DiscreteDeepPolicy;
class IMinibatch;
class ConfigNode;

class DQN : public Simion, DeferredLoad
{
	/*
	Deep Reinforcement Learning with Double Deep Q-Learning
	Hado van Hasselt and Arthur Guez and David Silver
	Proceedings of the Thirtieth AAAI Conference on Artificial Intelligence (AAAI-16)
	*/
protected:
	CHILD_OBJECT<DeepDiscreteQFunction> m_pQFunction;
	IDiscreteQFunctionNetwork* m_pTargetQNetwork = nullptr;
	IDiscreteQFunctionNetwork* m_pOnlineQNetwork = nullptr;
	DeepMinibatch* m_pMinibatch = nullptr;

	vector<double> m_Q_s_p;
	vector<double> m_maxQValue;
	vector<int> m_argMaxIndex;

	CHILD_OBJECT_FACTORY<DiscreteDeepPolicy> m_policy;

	virtual IDiscreteQFunctionNetwork* getTargetNetwork();
	
public:
	~DQN();
	DQN(ConfigNode *pParameters);

	virtual void deferredLoadStep();

	//selects an according to the learned policy pi(a|s)
	virtual double selectAction(const State *s, Action *a);

	//updates the critic and the actor
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb);
};

class DoubleDQN : public DQN
{
public:
	DoubleDQN(ConfigNode* pParameters);
	
	virtual IDiscreteQFunctionNetwork* getTargetNetwork();
};

#endif