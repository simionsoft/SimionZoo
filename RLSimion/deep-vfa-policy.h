#pragma once

#include "policy.h"
#include "parameters-numeric.h"

class DiscreteDeepPolicy
{
protected:

public:
	static std::shared_ptr<DiscreteDeepPolicy> DiscreteDeepPolicy::getInstance(ConfigNode* pConfigNode);

	DiscreteDeepPolicy(ConfigNode* pConfigNode);

	virtual int selectAction(const std::vector<double>& values) = 0;
};

class DiscreteEpsilonGreedyDeepPolicy : public DiscreteDeepPolicy
{
protected:
	CHILD_OBJECT_FACTORY<NumericValue> m_epsilon;
public:
	DiscreteEpsilonGreedyDeepPolicy(ConfigNode* pConfigNode);

	virtual int selectAction(const std::vector<double>& values);
};

class DiscreteSoftmaxDeepPolicy : public DiscreteDeepPolicy
{
protected:
	CHILD_OBJECT_FACTORY<NumericValue> m_temperature;
public:
	DiscreteSoftmaxDeepPolicy(ConfigNode* pConfigNode);

	virtual int selectAction(const std::vector<double>& values);
};
