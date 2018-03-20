#pragma once
#include "../reward.h"
class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;
typedef NamedVarSet Reward;
class Descriptor;

class DistanceReward2D : public IRewardComponent
{
	int m_var1xId, m_var1yId, m_var2xId, m_var2yId;
	double m_maxDist= 1.0;
public:
	DistanceReward2D(Descriptor& stateDescr, const char* var1xName, const char* var1yName, const char* var2xName, const char* var2yName);
	DistanceReward2D(Descriptor& stateDescr, int var1xId, int var1yId, int var2xId, int var2yId);
	double getReward(const State *s, const Action *a, const State *s_p);
	const char* getName() { return "reward"; }
	double getMin();
	double getMax();
};