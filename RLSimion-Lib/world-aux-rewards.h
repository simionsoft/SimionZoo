#pragma once
#include "reward.h"
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;
class CDescriptor;

class CDistanceReward2D : public IRewardComponent
{
	int m_var1xId, m_var1yId, m_var2xId, m_var2yId;
	double m_maxDist= 1.0;
public:
	CDistanceReward2D(CDescriptor& stateDescr, const char* var1xName, const char* var1yName, const char* var2xName, const char* var2yName);
	CDistanceReward2D(CDescriptor& stateDescr, int var1xId, int var1yId, int var2xId, int var2yId);
	double getReward(const CState *s, const CAction *a, const CState *s_p);
	const char* getName() { return "reward"; }
	double getMin();
	double getMax();
};