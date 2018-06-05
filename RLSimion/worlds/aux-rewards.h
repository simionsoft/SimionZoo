#pragma once
#include "../reward.h"
class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;
typedef NamedVarSet Reward;
class Descriptor;

class DistanceReward2D : public IRewardComponent
{
	const char *m_var1xId = nullptr, *m_var1yId = nullptr, *m_var2xId = nullptr, *m_var2yId = nullptr;
	double m_maxDist= 1.0;
public:
	DistanceReward2D(Descriptor& stateDescr, const char* var1xName, const char* var1yName, const char* var2xName, const char* var2yName);
	double getReward(const State *s, const Action *a, const State *s_p);
	const char* getName() { return "reward"; }
	double getMin();
	double getMax();
};