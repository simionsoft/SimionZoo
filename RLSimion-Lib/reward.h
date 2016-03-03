#pragma once
#include "parameterized-object.h"

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;
class CRewardFunctionComponent;

class CParameters;

class CRewardFunction: public CParamObject
{
	CRewardFunctionComponent** m_pRewardComponents;
	int m_numRewardComponents;

	double m_lastReward;
	double m_reward;
	CReward *m_pReward;
public:
	static double m_minReward;
	static double m_maxReward;

	CRewardFunction(CParameters* pParameters);
	~CRewardFunction();

	CReward* getReward();

	double calculateReward(CState *s,CAction *a,CState *s_p);

	double getLastScalarReward(){ return m_lastReward; }
	
	int getNumRewardComponents(){ return m_numRewardComponents; }
	double getLastRewardComponent(int i);
};