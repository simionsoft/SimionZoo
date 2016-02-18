#pragma once
#include "parameterized-object.h"

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CRewardComponent;

class tinyxml2::XMLElement;

class CReward: public CParamObject
{
	CRewardComponent** m_pRewardComponents;
	int m_numRewardComponents;

	double m_lastReward;
	
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