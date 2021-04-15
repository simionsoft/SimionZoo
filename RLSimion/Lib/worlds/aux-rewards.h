#pragma once
#include "../reward.h"
#include "drone-6-dof.h"
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

class DistanceReward3D : public IRewardComponent
{
	const char *m_errorVarNameX = nullptr, * m_errorVarNameY = nullptr, * m_errorVarNameZ = nullptr;
	double m_maxDist = 1.0;

public:
	//7 parametros de x y z x y rot x rot y
	DistanceReward3D(Descriptor& stateDescr, const char* errorVar1Name, const char* errorVar2Name, const char* errorVar3Name, double toleranceDistance);
	double getReward(const State *s, const Action *a, const State *s_p);
	const char* getName() { return "reward"; }
	double getMin();
	double getMax();
};