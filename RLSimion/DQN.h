#pragma once
#ifdef _WIN64
#include <vector>
#include "simion.h"
#include "critic.h"
#include "parameters.h"
#include "q-learners.h"
#include "deep-vfa-policy.h"
#include "experience-replay.h"

class SingleDimensionDiscreteActionVariableGrid;
class ExperienceReplay;
class ExperienceTuple;
class INetwork;

class DQN : public Simion
{
protected:
	ACTION_VARIABLE m_outputActionIndex;
	CHILD_OBJECT<ExperienceReplay> m_experienceReplay;
	NEURAL_NETWORK m_pTargetQNetwork;
	INetwork* m_pPredictionQNetwork= nullptr;

	CHILD_OBJECT_FACTORY<DiscreteDeepPolicy> m_policy;
	FeatureList* m_pStateOutFeatures;
	SingleDimensionDiscreteActionVariableGrid* m_pGrid;

	size_t m_numberOfActions;
	size_t m_numberOfStateFeatures;
	std::vector<double> m_stateVector;
	std::vector<double> m_actionValuePredictionVector;

	std::vector<double> m_minibatchStateVector;
	std::vector<double> m_minibatchActionValuePredictionVector;

	int* m_pMinibatchChosenActionIndex;

	ExperienceTuple** m_pMinibatchExperienceTuples;
	double* m_pMinibatchChosenActionTargetValues;

	INetwork* getPredictionNetwork();
	
public:
	~DQN();
	DQN(ConfigNode *pParameters);

	//selects an according to the learned policy pi(a|s)
	virtual double selectAction(const State *s, Action *a);

	//updates the critic and the actor
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb);
};

class DoubleDQN : public DQN
{
protected:
	INetwork* getPredictionNetwork();
public:
	//updates the target network and sometimes the prediction network
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb);
};

class DDPG : public Simion
{
protected:
	ACTION_VARIABLE m_outputActionIndex;
	CHILD_OBJECT<ExperienceReplay> m_experienceReplay;

	NEURAL_NETWORK m_predictionQNetwork;
	INetwork* m_pTargetQNetwork;

	NEURAL_NETWORK m_predictionPolicyNetwork;
	INetwork* m_pTargetPolicyNetwork;

	CHILD_OBJECT_FACTORY<Noise> m_policyNoise;
	DOUBLE_PARAM m_tau;

	ExperienceTuple** m_pMinibatchExperienceTuples;

	std::vector<double> m_stateVector;
	std::vector<double> m_actionPredictionVector;

	std::vector<double> m_minibatchStateVector;
	std::vector<double> m_minibatchActionVector;
	std::vector<double> m_minibatchQVector;

	FeatureList* m_pStateFeatureList;
	int m_numberOfStateFeatures;

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

#endif
