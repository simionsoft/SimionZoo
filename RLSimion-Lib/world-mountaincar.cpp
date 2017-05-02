#include "stdafx.h"
#include "world-mountaincar.h"
#include "named-var-set.h"
#include "config.h"
#include "reward.h"
#include "app.h"

CMountainCar::CMountainCar(CConfigNode* pConfigNode)
{
	METADATA("World", "Mountain-car");
	m_sPosition = addStateVariable("position","m",-1.2,0.6);
	m_sVelocity = addStateVariable("velocity","m/s",-0.07,0.07);

	m_aPedal = addActionVariable("pedal","m",-1.0,1.0);

	//the reward function
	m_pRewardFunction->addRewardComponent(new CMountainCarReward());
	m_pRewardFunction->initialize();
}

CMountainCar::~CMountainCar()
{

}

void CMountainCar::reset(CState *s)
{

	//if (CSimionApp::getSample()->pExperiment->isEvaluationEpisode())
	{
		//fixed setting in evaluation episodes
		s->set(m_sPosition, -0.5);
		s->set(m_sVelocity, 0.0);
	}
	//else
	//{
	//	//random setting in training episodes
	//	s->set(m_sPosition, -1.0 + getRandomValue()*0.8); //[-1.0, -0.2]
	//	s->set(m_sVelocity, -0.02 + getRandomValue()*0.04); //[-0.02,0.02]
	//}
}


void CMountainCar::executeAction(CState *s, const CAction *a, double dt)
{
	//this simulation model ignores dt!!

	double position= s->get(m_sPosition);
	double velocity = s->get(m_sVelocity);
	double pedal = a->get(m_aPedal);

	/*
	mcar_velocity += (a-1)*0.001 + cos(3*mcar_position)*(-0.0025);
    if (mcar_velocity > mcar_max_velocity) mcar_velocity = mcar_max_velocity;
    if (mcar_velocity < -mcar_max_velocity) mcar_velocity = -mcar_max_velocity;
    mcar_position += mcar_velocity;
    if (mcar_position > mcar_max_position) mcar_position = mcar_max_position;
    if (mcar_position < mcar_min_position) mcar_position = mcar_min_position;
    if (mcar_position==mcar_min_position && mcar_velocity<0) mcar_velocity = 0;}
	*/
	if (position > s->getProperties(m_sPosition).getMin())
	{
		velocity += pedal*0.001 + cos(3 * position)*(-0.0025);
		s->set(m_sVelocity, velocity); //saturate
		velocity = s->get(m_sVelocity);
		position += velocity;
	}
	else
		velocity = 0.0;

	s->set(m_sPosition, position);
	s->set(m_sVelocity, velocity);
}


double CMountainCarReward::getReward(const CState* s, const CAction* a, const CState* s_p)
{
	double position = s_p->get("position");

	//reached the goal?
	if (position == s_p->getProperties("position").getMax())
	{
		CSimionApp::get()->pExperiment->setTerminalState();
		/*printf("goal reached\n");*/
		return 1.0;
	}
	//reached the minimum position to the left?
	if (position == s_p->getProperties("position").getMin())
	{
		CSimionApp::get()->pExperiment->setTerminalState();
		return -100.0;
	}
	return -1.0;
}

double CMountainCarReward::getMin() { return -100.0; }

double CMountainCarReward::getMax() { return 1.0; }