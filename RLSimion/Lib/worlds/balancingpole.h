#pragma once

#include "world.h"
class SetPoint;
#include "../reward.h"

//Original implementation in C by Sutton et al can be found in:
//https://webdocs.cs.ualberta.ca/~sutton/book/code/pole.c

//BALANCING POLE
class BalancingPole: public DynamicModel
{
	double GRAVITY;
	double MASSCART;
	double MASSPOLE;
	double TOTAL_MASS;
	double LENGTH;
	double POLEMASS_LENGTH;

	size_t m_sX, m_sX_dot;
	size_t m_sTheta, m_sTheta_dot;
	size_t m_aPitch;

public:
	BalancingPole(ConfigNode* pParameters);
	virtual ~BalancingPole() = default;

	void reset(State *s);

	void executeAction(State *s, const Action *a, double dt);
};

class BalancingPoleReward : public IRewardComponent
{
public:
	double getReward(const State *s, const Action *a, const State *s_p);
	const char* getName(){ return "reward"; }
	double getMin();
	double getMax();
};