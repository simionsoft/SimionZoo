#pragma once
#ifdef _WIN64

#include <vector>
#include "simion.h"
#include "parameters.h"
#include "deep-vfa-policy.h"


class SingleDimensionDiscreteActionVariableGrid;
class ExperienceReplay;
class ExperienceTuple;
/*
class AsyncQLearning : public Simion
{
protected:
	ACTION_VARIABLE m_outputActionIndex;
	CHILD_OBJECT<ExperienceReplay> m_experienceReplay;
	NN_DEFINITION m_predictionQNetwork;
	
	CHILD_OBJECT_FACTORY<DiscreteDeepPolicy> m_policy;
	FeatureList* m_pStateOutFeatures;
	SingleDimensionDiscreteActionVariableGrid* m_pGrid;

	int m_numberOfActions;
	int m_numberOfStateVars;
	std::vector<double> m_stateVector;
	std::vector<double> m_actionValuePredictionVector;

	std::vector<double> m_minibatchStateVector;
	std::vector<double> m_minibatchActionValuePredictionVector;

	int* m_pMinibatchChosenActionIndex;

	ExperienceTuple** m_pMinibatchExperienceTuples;
	double* m_pMinibatchChosenActionTargetValues;
	
	INT_PARAM m_iTarget;
	INT_PARAM m_iAsyncUpdate;

	INetwork* m_pTargetQNetwork;

	unsigned int m_aggregatedGradientCount;
	std::unordered_map<CNTK::Parameter, CNTK::NDArrayViewPtr> m_aggregatedGradientValues;
	std::unordered_map<CNTK::Parameter, CNTK::ValuePtr> m_aggregatedGradients;

public:
	AsyncQLearning(ConfigNode *pParameters);
	~AsyncQLearning();

	//selects an according to the selected policy and the prediction of the Q network
	virtual double selectAction(const State *s, Action *a);

	//updates the target network and sometimes the prediction network
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb);
};*/
#endif

