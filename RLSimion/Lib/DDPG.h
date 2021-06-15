#pragma once
#if defined(__linux__) || defined(_WIN64)
#include "simion.h"
#include "deferred-load.h"

class Noise;
class IDeepNetwork;
class DeepMinibatch;
class DeepDeterministicPolicy;
class DeepContinuousQFunction;
class IContinuousQFunctionNetwork;
class IDeterministicPolicyNetwork;

//This class implements Deep Deterministic Policy Gradient (DDPG)
//Original paper: https://arxiv.org/abs/1509.02971

//TensorFlow implementations:
//https://github.com/floodsung/DDPG
//https://gym.openai.com/evaluations/eval_GH4gO123SvukqlMmqhIyeQ/
//https://github.com/agakshat/tensorflow-ddpg
//Keras implementation:
//https://yanpanlau.github.io/2016/10/11/Torcs-Keras.html

class DDPG : public Simion, DeferredLoad
{
	//Actor
	DeepMinibatch* m_pActorMinibatch = nullptr;
	IDeterministicPolicyNetwork* m_pActorOnlineNetwork = nullptr;
	IDeterministicPolicyNetwork* m_pActorTargetNetwork = nullptr;
	CHILD_OBJECT<DeepDeterministicPolicy> m_actorPolicy;
	MULTI_VALUE_FACTORY<Noise> m_noiseSignals;
	vector<double> m_pi_s;
	vector<double> m_pi_s_p;
	vector<double> m_gradient;
	//Critic
	DeepMinibatch* m_pCriticMinibatch = nullptr;
	IContinuousQFunctionNetwork* m_pCriticOnlineNetwork = nullptr;
	IContinuousQFunctionNetwork* m_pCriticTargetNetwork = nullptr;
	CHILD_OBJECT<DeepContinuousQFunction> m_criticQFunction;
	vector<double> m_Q_pi_s_p;

	DOUBLE_PARAM m_tau;

	size_t m_numUpdates = 0;
	size_t m_targetFunctionUpdateFreq = 10;

	//update policy network
	void updateActor(const State* s, const Action* a, const State* s_p, double r);

	//update q network
	void updateCritic(const State* s, const Action* a, const State* s_p, double r);

public:
	~DDPG();
	DDPG(ConfigNode* pParameters);

	//heavy-weight initialization
	virtual void deferredLoadStep();

	//selects an action according to the learned policy's network
	virtual double selectAction(const State *s, Action *a);

	//updates the critic network and the actor's policy network (both the target and the prediction network)
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb);
};

#endif