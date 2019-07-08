#pragma once

#include "simion.h"
#include "parameters.h"
#include "parameters-numeric.h"
#include "deep-function.h"

class DeepCACLA : public Simion
{
	CHILD_OBJECT<DeepDeterministicPolicy> m_actorPolicy;
	CHILD_OBJECT<DeepVFunction> m_criticFunction;
public:
	DeepCACLA(ConfigNode* pConfigNode);
	virtual ~DeepCACLA();

	virtual double update(const State *s, const Action *a, const State *s_p, double r, double probability);

	virtual double selectAction(const State *s, Action *a);
};

