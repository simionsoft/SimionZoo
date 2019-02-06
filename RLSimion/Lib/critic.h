#pragma once
#include "parameters.h"

class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;

class ConfigNode;
class LinearStateVFA;

class ICritic
{
public:
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double rho = 1.0) = 0;

	static std::shared_ptr<ICritic> getInstance(ConfigNode* pParameters);
};

class VLearnerCritic: public ICritic
{

protected:
	CHILD_OBJECT<LinearStateVFA> m_pVFunction; //value function approximator

public:
	VLearnerCritic(ConfigNode* pParameters);
	virtual ~VLearnerCritic();

	virtual double update(const State *s, const Action *a, const State *s_p, double r, double rho= 1.0) = 0;

};

