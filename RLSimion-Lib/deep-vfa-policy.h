#pragma once
#ifdef _WIN64
#include "policy.h"
#include "parameters-numeric.h"

class CSingleDimensionDiscreteActionVariableGrid;

class CDeepPolicy
{
protected:
	ACTION_VARIABLE m_outputActionIndex;
	NEURAL_NETWORK_PROBLEM_DESCRIPTION m_QNetwork;
	shared_ptr<CNetwork> m_pTargetNetwork;

public:
	CDeepPolicy(CConfigNode* pConfigNode);
	~CDeepPolicy();

	static std::shared_ptr<CDeepPolicy> CDeepPolicy::getInstance(CConfigNode* pConfigNode);

	virtual double selectAction(const CState *s, CAction *a) = 0;
	virtual double updateNetwork(const CState * s, const CAction * a, const CState * s_p, double r) = 0;
};

class CDiscreteEpsilonGreedyDeepPolicy : public CDeepPolicy
{
protected:
	CHILD_OBJECT_FACTORY<CNumericValue> m_epsilon;
	CSingleDimensionDiscreteActionVariableGrid* m_pGrid;
	CFeatureList* m_pStateOutFeatures;

	int m_numberOfActions;
	int m_numberOfStateFeatures;
	std::vector<float> m_stateVector;
	std::vector<float> m_actionValuePredictionVector;
public:
	CDiscreteEpsilonGreedyDeepPolicy(CConfigNode* pConfigNode);

	virtual double selectAction(const CState *s, CAction *a);
	virtual double updateNetwork(const CState * s, const CAction * a, const CState * s_p, double r);
};
#endif