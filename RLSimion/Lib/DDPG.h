#pragma once
#if defined(__linux__) || defined(_WIN64)
#include "simion.h"
#include "deferred-load.h"

class Noise;
class INetwork;
class IMinibatch;

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
	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_inputState;
	MULTI_VALUE_VARIABLE<ACTION_VARIABLE> m_outputAction;
	DOUBLE_PARAM m_learningRate;

	NN_DEFINITION m_CriticNetworkDefinition;
	INetwork* m_pCriticOnlineNetwork= nullptr;
	INetwork* m_pCriticTargetNetwork= nullptr;
	vector<double> m_criticTarget;

	NN_DEFINITION m_ActorNetworkDefinition;
	INetwork* m_pActorOnlineNetwork= nullptr;
	INetwork* m_pActorTargetNetwork= nullptr;
	vector<double> m_actorTarget;

	IMinibatch* m_pMinibatch = nullptr;
	size_t m_minibatchSize;

	vector<double> m_pi_s;
	vector<double> m_pi_s_p;
	vector<double> m_Q_pi_s_p;
	
	vector<double> m_gradientWrtAction;

	CHILD_OBJECT_FACTORY<Noise> m_policyNoise;
	DOUBLE_PARAM m_tau;

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