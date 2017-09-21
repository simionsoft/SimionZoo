#pragma once
#ifdef _WIN64
#include "policy.h"
#include "parameters-numeric.h"

class CDiscreteDeepPolicy
{
protected:

public:
	static std::shared_ptr<CDiscreteDeepPolicy> CDiscreteDeepPolicy::getInstance(CConfigNode* pConfigNode);

	CDiscreteDeepPolicy(CConfigNode* pConfigNode);

	virtual int selectAction(std::vector<float> values) = 0;
};

class CDiscreteEpsilonGreedyDeepPolicy : public CDiscreteDeepPolicy
{
protected:
	CHILD_OBJECT_FACTORY<CNumericValue> m_epsilon;
public:
	CDiscreteEpsilonGreedyDeepPolicy(CConfigNode* pConfigNode);

	virtual int selectAction(std::vector<float> values);
};

class CDiscreteSoftmaxDeepPolicy : public CDiscreteDeepPolicy
{
protected:
	CHILD_OBJECT_FACTORY<CNumericValue> m_temperature;
public:
	CDiscreteSoftmaxDeepPolicy(CConfigNode* pConfigNode);

	virtual int selectAction(std::vector<float> values);
};
#endif