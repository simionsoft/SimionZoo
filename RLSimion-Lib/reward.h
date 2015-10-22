#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CErrorComponent;

class CReward
{
	CErrorComponent* m_pErrorComponents;
	double m_lastReward;
	double m_minReward;
	double m_maxReward;
	int m_numRewardComponents;
public:
	CReward(char* configFile);
	~CReward();

	double getReward(CState *s,CAction *a,CState *s_p);
};