#pragma once
#include "parameterized-object.h"

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;
class CRewardFunctionComponent;

#include <vector>

class CParameters;

class IRewardComponent
{
public:
	virtual double getReward(const CState *s, const CAction* a, const CState *s_p, bool& bFailureState) = 0;
	virtual const char* getName()= 0;
};


#define MAX_REWARD_NAME_SIZE 128
class CToleranceRegionReward: public IRewardComponent
{
	char m_name[MAX_REWARD_NAME_SIZE];
	char* m_pVariableName;
	double m_tolerance;
	double m_scale;
	double m_lastReward;

public:
	const double m_minReward = -1.0;
	const double m_maxReward = 1.0;

	CToleranceRegionReward(char* variable, double tolerance, double scale);
	double getReward(const CState *s, const CAction* a, const CState *s_p, bool& bFailureState);
	const char* getName();
};

class CRewardFunction
{
	std::vector<IRewardComponent*> m_rewardComponents;

	//after calling getReward(), the full reward vector is stored here in case we want to observe the different components
	CReward* m_pRewardVector;
	bool m_bInitialized;
public:

	void addRewardComponent(IRewardComponent *rewardComponent);
	void initialize();

	CRewardFunction();
	virtual ~CRewardFunction();

	CReward* getRewardVector();
	double getReward(const CState *s, const CAction *a, const CState *s_p, bool& bFailureState);
	
	//int getNumRewardComponents(){ return m_rewardComponents.size(); }
	//double getLastRewardComponent(int i);
};