#pragma once
#ifdef _WIN64

#include "simion.h"
#include "deferred-load.h"

class Noise;
class INetwork;
class IMinibatch;

class DDPG : public Simion, DeferredLoad
{
protected:
	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_inputState;
	ACTION_VARIABLE m_outputAction;
	DOUBLE_PARAM m_learningRate;

	NN_DEFINITION m_CriticNetworkDefinition;
	INetwork* m_pCriticOnlineNetwork= nullptr;
	INetwork* m_pCriticTargetNetwork= nullptr;
	IMinibatch* m_pCriticMinibatch= nullptr;

	NN_DEFINITION m_ActorNetworkDefinition;
	INetwork* m_pActorOnlineNetwork= nullptr;
	INetwork* m_pActorTargetNetwork= nullptr;
	IMinibatch* m_pActorMinibatch= nullptr;

	vector<double> m_stateValues;
	vector<double> m_actionValues;
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