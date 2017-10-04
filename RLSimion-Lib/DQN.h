#pragma once
#ifdef _WIN64
#include <vector>
#include "simion.h"
#include "critic.h"
#include "parameters.h"
#include "q-learners.h"
#include "deep-vfa-policy.h"
#include "experience-replay.h"

class CSingleDimensionDiscreteActionVariableGrid;
class CExperienceReplay;
class CExperienceTuple;


class CDQN : public CSimion
{
protected:
	ACTION_VARIABLE m_outputActionIndex;
	CHILD_OBJECT<CExperienceReplay> m_experienceReplay;
	NEURAL_NETWORK_PROBLEM_DESCRIPTION m_QNetwork;
	shared_ptr<CNetwork> m_pPredictionNetwork;

	CHILD_OBJECT_FACTORY<CDiscreteDeepPolicy> m_policy;
	CFeatureList* m_pStateOutFeatures;
	CSingleDimensionDiscreteActionVariableGrid* m_pGrid;

	size_t m_numberOfActions;
	size_t m_numberOfStateFeatures;
	std::vector<float> m_stateVector;
	std::vector<float> m_actionValuePredictionVector;

	std::vector<float> m_minibatchStateVector;
	std::vector<float> m_minibatchActionValuePredictionVector;

	int* m_pMinibatchChosenActionIndex;

	CExperienceTuple** m_pMinibatchExperienceTuples;
	double* m_pMinibatchChosenActionTargetValues;

	std::shared_ptr<CNetwork> getPredictionNetwork();
	
public:
	~CDQN();
	CDQN(CConfigNode *pParameters);

	//selects an according to the learned policy pi(a|s)
	virtual double selectAction(const CState *s, CAction *a);

	//updates the critic and the actor
	virtual double update(const CState *s, const CAction *a, const CState *s_p, double r, double behaviorProb);
};
#endif
