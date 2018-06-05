#include "raincar.h"
#include "../named-var-set.h"
#include "../config.h"
#include "../reward.h"
#include "../experiment.h"
#include <algorithm>


RainCar::RainCar(ConfigNode* pConfigNode)
{
	METADATA("World", "Rain-car");

	addConstant("goal-position", 24.0);

	m_sPosition = addStateVariable("position", "m", 0.0, 50.0);
	m_sVelocity = addStateVariable("velocity", "m/s", -5.0, 5.0);
	m_sPositionDeviation = addStateVariable("position-deviation", "m", -25.0, 25.0);

	m_aAcceleration = addActionVariable("acceleration", "m", -1.0, 1.0);

	//the reward function
	m_pRewardFunction->addRewardComponent(new RainCarReward());
	m_pRewardFunction->initialize();
}

RainCar::~RainCar()
{

}

void RainCar::reset(State *s)
{
	s->set(m_sPosition, 0.0);
	s->set(m_sVelocity, 0.0);
	s->set(m_sPositionDeviation, getConstant("goal-position"));
}

void RainCar::executeAction(State *s, const Action *a, double dt)
{
	double position = s->get(m_sPosition);
	double velocity = s->get(m_sVelocity);
	double acceleration = a->get(m_aAcceleration);

	if (position <= s->getProperties(m_sPosition)->getMin() && velocity < 0 || position >= s->getProperties(m_sPosition)->getMax() && velocity > 0)
	{
		velocity = 0;
	}

	s->set(m_sVelocity, velocity + acceleration*dt);
	s->set(m_sPosition, position + velocity * dt);
	s->set(m_sPositionDeviation, getConstant("goal-position") - s->get(m_sPosition));
}


double RainCarReward::getReward(const State* s, const Action* a, const State* s_p)
{
	double position = s_p->get("position");
	if ((position == s->getProperties("position")->getMin() && a->get((size_t)0) < 0.0)
		|| (position == s->getProperties("position")->getMax() && a->get((size_t)0) > 0.0))
		return -10;
	double targetPosition = 24.0;

	double reward= 1.0 - abs(targetPosition-position);
	reward = std::max(-1.0, reward);

	return reward;
}

double RainCarReward::getMin() { return -10.0; }

double RainCarReward::getMax() { return 1.0; }