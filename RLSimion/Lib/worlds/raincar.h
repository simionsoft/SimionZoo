#pragma once

#include "world.h"
class SetPoint;
#include "../reward.h"

//Original implementation:
//http://incompleteideas.net/MountainCar/MountainCar.html
//Equations for variables used in visualization from:
//https://code.google.com/archive/p/rl-library/downloads

//MOUNTAIN CAR
class RainCar: public DynamicModel
{
	size_t m_sVelocity, m_sPosition, m_sPositionDeviation;

	size_t m_aAcceleration;

public:
	RainCar(ConfigNode* pParameters);
	virtual ~RainCar();

	void reset(State *s);

	void executeAction(State *s, const Action *a, double dt);
};

class RainCarReward : public IRewardComponent
{
public:
	RainCarReward() = default;
	double getReward(const State *s, const Action *a, const State *s_p);
	const char* getName(){ return "reward"; }
	double getMin();
	double getMax();
};