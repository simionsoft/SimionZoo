#pragma once
#ifdef _WIN64
#include <vector>
#include "simion.h"
#include "parameters.h"
#include "deferred-load.h"

class INetwork;
class SingleDimensionDiscreteActionVariableGrid;
class DiscreteDeepPolicy;
class ConfigNode;

class DQN : public Simion, DeferredLoad
{
protected:
	MULTI_VALUE_VARIABLE<STATE_VARIABLE> m_inputState;
	INT_PARAM m_numActionSteps;
	ACTION_VARIABLE m_outputAction;

	NN_DEFINITION m_pNNDefinition;
	INetwork* m_pTargetQNetwork= nullptr;
	INetwork* m_pOnlineQNetwork= nullptr;

	CHILD_OBJECT_FACTORY<DiscreteDeepPolicy> m_policy;

	SingleDimensionDiscreteActionVariableGrid* m_pGrid= nullptr;

	size_t m_numberOfActions= 0;
	size_t m_numberOfStateVars= 0;
	std::vector<double> m_stateVector;
	std::vector<double> m_actionValuePredictionVector;

	std::vector<double> m_minibatch_s;
	std::vector<double> m_minibatch_Q_s;

	size_t m_minibatchTuples = 0;
	size_t m_minibatchSize = 0;
	size_t* m_pMinibatchActionId= nullptr;

	double* m_pMinibatchChosenActionTargetValues= nullptr;

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
