#pragma once
#include "parameters.h"
class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;

class ConfigNode;

class Simion
{
	ENUM_PARAM<UseAsController> m_useAsController;
public:
	Simion(ConfigNode* pParameters);
	virtual ~Simion(){};

	UseAsController getUseAsController() { return m_useAsController.get(); }

	virtual double update(const State *s, const Action *a, const State *s_p, double r, double probability) = 0;

	//selectAction sets output in a, and returns the probability under which the simion selected the action
	virtual double selectAction(const State *s, Action *a) = 0;

	static std::shared_ptr<Simion> getInstance(ConfigNode* pParameters);
};