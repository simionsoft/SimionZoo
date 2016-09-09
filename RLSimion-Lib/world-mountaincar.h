#pragma once

#include "world.h"
class CSetPoint;
#include "reward.h"

//Original implementation:
//https://webdocs.cs.ualberta.ca/~sutton/MountainCar/MountainCar.html

//MOUNTAIN CAR
class CMountainCar: public CDynamicModel
{
	int m_sVelocity, m_sPosition;
	int m_aPedal;

public:
	CMountainCar(CParameters* pParameters, const char* worldDefinition);
	virtual ~CMountainCar();

	void reset(CState *s);

	void executeAction(CState *s, const CAction *a, double dt);
};

class CMountainCarReward : public IRewardComponent
{
public:
	double getReward(const CState *s, const CAction *a, const CState *s_p);
	const char* getName(){ return "reward"; }
};