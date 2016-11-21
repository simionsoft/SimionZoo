#pragma once


#include "named-var-set.h"
class CRewardFunctionComponent;

#include <vector>

class CConfigNode;

class IRewardComponent
{
public:
	virtual double getReward(const CState *s, const CAction* a, const CState *s_p) = 0;
	virtual const char* getName()= 0;
	virtual double getMin() = 0;
	virtual double getMax() = 0;
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
	double getReward(const CState *s, const CAction* a, const CState *s_p);
	const char* getName();
	double getMin() { return m_minReward; }
	double getMax() { return m_maxReward; }
};

class CRewardFunction
{
	std::vector<IRewardComponent*> m_rewardComponents;

	CDescriptor rewardDescriptor;
	CReward* m_pRewardVector;
	bool m_bInitialized;
public:

	void addRewardComponent(IRewardComponent *rewardComponent);
	void initialize();

	CRewardFunction();
	virtual ~CRewardFunction();

	CReward* getRewardVector();
	double getReward(const CState *s, const CAction *a, const CState *s_p);
	
	//int getNumRewardComponents(){ return m_rewardComponents.size(); }
	//double getLastRewardComponent(int i);
};