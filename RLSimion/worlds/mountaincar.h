#pragma once

#include "world.h"
class SetPoint;
#include "../reward.h"

//Original implementation:
//https://webdocs.cs.ualberta.ca/~sutton/MountainCar/MountainCar.html

//MOUNTAIN CAR
class MountainCar: public DynamicModel
{
	int m_sVelocity, m_sPosition;
	int m_aPedal;

public:
	MountainCar(ConfigNode* pParameters);
	virtual ~MountainCar();

	void reset(State *s);

	void executeAction(State *s, const Action *a, double dt);
};

class MountainCarReward : public IRewardComponent
{
public:
	MountainCarReward() = default;
	double getReward(const State *s, const Action *a, const State *s_p);
	const char* getName(){ return "reward"; }
	double getMin();
	double getMax();
};