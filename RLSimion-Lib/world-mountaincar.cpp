#include "stdafx.h"
#include "world-mountaincar.h"
#include "named-var-set.h"
#include "globals.h"
#include "parameters.h"
#include "reward.h"
#include "app.h"
#include "noise.h"

CLASS_CONSTRUCTOR(CMountainCar, const char* worldDefinition)
: CDynamicModel(worldDefinition)
{
	CState *pStateDescriptor = getStateDescriptor();
	m_sPosition = pStateDescriptor->getVarIndex("position");
	m_sVelocity = pStateDescriptor->getVarIndex("velocity");

	CAction *pActionDescriptor = getActionDescriptor();
	m_aPedal = pActionDescriptor->getVarIndex("pedal");

	//the reward function
	m_pRewardFunction->addRewardComponent(new CMountainCarReward());
	m_pRewardFunction->initialize();
	
	END_CLASS();
}

CMountainCar::~CMountainCar()
{

}

void CMountainCar::reset(CState *s)
{

	//if (CApp::get()->Experiment.isEvaluationEpisode())
	{
		//fixed setting in evaluation episodes
		s->setValue(m_sPosition, -0.5);
		s->setValue(m_sVelocity, 0.0);
	}
	//else
	//{
	//	//random setting in training episodes
	//	s->setValue(m_sPosition, -1.0 + getRandomValue()*0.8); //[-1.0, -0.2]
	//	s->setValue(m_sVelocity, -0.02 + getRandomValue()*0.04); //[-0.02,0.02]
	//}
}


void CMountainCar::executeAction(CState *s, const CAction *a, double dt)
{
	//this simulation model ignores dt!!

	double position= s->getValue(m_sPosition);
	double velocity = s->getValue(m_sVelocity);
	double pedal = a->getValue(m_aPedal);

	/*
	mcar_velocity += (a-1)*0.001 + cos(3*mcar_position)*(-0.0025);
    if (mcar_velocity > mcar_max_velocity) mcar_velocity = mcar_max_velocity;
    if (mcar_velocity < -mcar_max_velocity) mcar_velocity = -mcar_max_velocity;
    mcar_position += mcar_velocity;
    if (mcar_position > mcar_max_position) mcar_position = mcar_max_position;
    if (mcar_position < mcar_min_position) mcar_position = mcar_min_position;
    if (mcar_position==mcar_min_position && mcar_velocity<0) mcar_velocity = 0;}
	*/
	if (position > s->getMin(m_sPosition))
	{
		velocity += pedal*0.001 + cos(3 * position)*(-0.0025);
		s->setValue(m_sVelocity, velocity); //saturate
		velocity = s->getValue(m_sVelocity);
		position += velocity;
	}
	else
		velocity = 0.0;

	s->setValue(m_sPosition, position);
	s->setValue(m_sVelocity, velocity);
}


double CMountainCarReward::getReward(const CState* s, const CAction* a, const CState* s_p)
{
	double position = s_p->getValue("position");

	//reached the goal?
	if (position == s_p->getMax("position"))
	{
		CApp::get()->Experiment.setTerminalState();
		printf("goal reached\n");
		return 1.0;
	}
	//reached the minimum position to the left?
	if (position == s_p->getMin("position"))
	{
		CApp::get()->Experiment.setTerminalState();
		return -100.0;
	}
	return -1.0;
}