#pragma once

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CErrorComponent;
class CParameters;
class tinyxml2::XMLElement;

class CReward
{
	CErrorComponent* m_pErrorComponents;
	double m_lastReward;
	
	int m_numRewardComponents;
public:
	static double m_minReward;
	static double m_maxReward;

	CReward(tinyxml2::XMLElement* pParameters);
	~CReward();

	double getReward(CState *s,CAction *a,CState *s_p);

	double getLastScalarReward(){ return m_lastReward; }
	
	int getNumRewardComponents(){ return m_numRewardComponents; }
	double getLastRewardComponent(int i);
};