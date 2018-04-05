#pragma once

#include "world.h"
class SetPoint;
#include "../reward.h"

//Original implementation:
//http://incompleteideas.net/MountainCar/MountainCar.html
//Equations for variables used in visualization from:
//https://code.google.com/archive/p/rl-library/downloads

//MOUNTAIN CAR
class MountainCar: public DynamicModel
{
	int m_sVelocity, m_sPosition;
	int m_sHeight, m_sAngle;
	int m_aPedal;
	double getHeightAtPos(double x);
	double getAngleAtPos(double x);
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