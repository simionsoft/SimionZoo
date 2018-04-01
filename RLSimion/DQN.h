#pragma once
#ifdef _WIN64
#include <vector>
#include "simion.h"
#include "parameters.h"
#include "deferred-load.h"

class INetwork;
class DiscreteDeepPolicy;
class IMinibatch;
class ConfigNode;

class DQN : public Simion, DeferredLoad
{
protected:
	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_inputState;
	INT_PARAM m_numActionSteps;
	ACTION_VARIABLE m_outputAction;

	vector<double> m_Q_s_p;
	vector<double> m_Q_s;

	NN_DEFINITION m_pNNDefinition;
	INetwork* m_pTargetQNetwork= nullptr;
	INetwork* m_pOnlineQNetwork= nullptr;
	IMinibatch* m_pMinibatch = nullptr;

	CHILD_OBJECT_FACTORY<DiscreteDeepPolicy> m_policy;

	size_t m_minibatchSize = 0;

	virtual INetwork* getPredictionNetwork();
	
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
	
	virtual INetwork* getPredictionNetwork();
};
/*
class DDPG : public Simion
{
protected:
	ACTION_VARIABLE m_outputAction;
	CHILD_OBJECT<ExperienceReplay> m_experienceReplay;

	NN_DEFINITION m_predictionQNetwork;
	INetwork* m_pTargetQNetwork;

	NN_DEFINITION m_predictionPolicyNetwork;
	INetwork* m_pTargetPolicyNetwork;

	CHILD_OBJECT_FACTORY<Noise> m_policyNoise;
	DOUBLE_PARAM m_tau;

	ExperienceTuple** m_pMinibatchExperienceTuples;

	std::vector<double> m_stateVector;
	std::vector<double> m_actionPredictionVector;

	std::vector<double> m_minibatch_s;
	std::vector<double> m_minibatchActionVector;
	std::vector<double> m_minibatchQVector;

	FeatureList* m_pStateFeatureList;
	int m_numberOfStateVars;

	void buildModelsParametersTransitionGraph();
	void performModelsParametersTransition();

	

public:
	~DDPG();
	DDPG(ConfigNode* pParameters);

	//selects an according to the learned policy's network
	virtual double selectAction(const State *s, Action *a);

	//updates the critic network and the actor's policy network (both the target and the prediction network)
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb);

	//update policy network
	void updateActor();

	//update q network
	void updateCritic();
};
*/
#endif
