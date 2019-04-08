#pragma once


#include "../Common/named-var-set.h"

#include <vector>
#include <string>
using namespace std;
class ConfigNode;

class IRewardComponent
{
public:
	IRewardComponent() = default;
	virtual ~IRewardComponent()= default;
	virtual double getReward(const State *s, const Action* a, const State *s_p) = 0;
	virtual const char* getName()= 0;
	virtual double getMin() = 0;
	virtual double getMax() = 0;
};


class ToleranceRegionReward: public IRewardComponent
{
	string m_name;
	string m_pVariableName;
	double m_tolerance;
	double m_scale;
	double m_lastReward;

public:
	double m_minReward = -1.0;
	double m_maxReward = 1.0;

	ToleranceRegionReward(string variable, double tolerance, double scale);
	double getReward(const State *s, const Action* a, const State *s_p);
	const char* getName();
	double getMin() { return m_minReward; }
	double getMax() { return m_maxReward; }
	void setMin(double newMin) { m_minReward = newMin; }
	void setMax(double newMax) { m_maxReward = newMax; }
};

class RewardFunction
{
	std::vector<IRewardComponent*> m_rewardComponents;

	Descriptor m_rewardDescriptor;
	Reward* m_pRewardVector;
	bool m_bInitialized;

	bool m_bOverride = false;
	double m_overrideValue = 0.0;
public:

	void addRewardComponent(IRewardComponent *rewardComponent);
	void initialize();

	RewardFunction();
	virtual ~RewardFunction();

	Reward* getRewardVector();
	Reward* getRewardInstance();
	double getReward(const State *s, const Action *a, const State *s_p);
	
	//This method can be used to, instead of calculating the next reward, return the given value
	//Useful in case something fails during the simulation (i.e., premature stop in FAST)
	void override(double reward);
};