#include "mountaincar.h"
#include "../named-var-set.h"
#include "../config.h"
#include "../reward.h"
#include "../app.h"
#include "../noise.h"
#include "../experiment.h"

#define HILL_PEAK_FREQ 3.0

MountainCar::MountainCar(ConfigNode* pConfigNode)
{
	METADATA("World", "Mountain-car");
	m_sPosition = addStateVariable("position", "m", -1.2, 0.6);
	m_sVelocity = addStateVariable("velocity", "m/s", -0.07, 0.07);
	m_sHeight = addStateVariable("height", "m", -1.0, 1.0);
	m_sAngle = addStateVariable("angle", "rad", -3.1415, 3.1415, true);

	m_aPedal = addActionVariable("pedal", "m", -1.0, 1.0);

	//the reward function
	m_pRewardFunction->addRewardComponent(new MountainCarReward());
	m_pRewardFunction->initialize();
}

MountainCar::~MountainCar()
{

}

double MountainCar::getHeightAtPos(double x)
{
	return sin(HILL_PEAK_FREQ * x);
}

double MountainCar::getAngleAtPos(double x)
{
	double slope = cos(HILL_PEAK_FREQ * x);
	return atan(slope);
}

void MountainCar::reset(State *s)
{
	//Here because the world needs to be initialized
	//Override delta_t and Num-Integration-Steps
	SimionApp::get()->pWorld->overrideDT(1.0);
	SimionApp::get()->pWorld->overrideNumIntegrationSteps(1);

	double x;
	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//fixed setting in evaluation episodes
		x = -0.5;
		s->set(m_sPosition, x);
	}
	else
	{
		//random setting in training episodes
		x = getRandomValue()*0.2 - 0.6;    //[-0.6, -0.4]
		s->set(m_sPosition, x); 
	}
	s->set(m_sVelocity, 0.0);
	s->set(m_sHeight, getHeightAtPos(x));
	s->set(m_sAngle, getAngleAtPos(x));
}


void MountainCar::executeAction(State *s, const Action *a, double dt)
{
	double position = s->get(m_sPosition);
	double velocity = s->get(m_sVelocity);
	double pedal = a->get(m_aPedal);

	if (position <= s->getProperties(m_sPosition).getMin() && velocity < 0)
		velocity = 0;

	if (m_sPosition!=-0.5 && pedal!=0.0)
		s->set(m_sVelocity, velocity + pedal*0.001 - 0.0025 * cos(3 * position));
	else s->set(m_sVelocity, 0.0);
	s->set(m_sPosition, position + velocity );
	s->set(m_sHeight, getHeightAtPos(s->get(m_sPosition)));
	s->set(m_sAngle, getAngleAtPos(s->get(m_sPosition)));
}


double MountainCarReward::getReward(const State* s, const Action* a, const State* s_p)
{
	double position = s_p->get("position");

	//reached the goal?
	if (position == s_p->getProperties("position").getMax())
	{
		SimionApp::get()->pExperiment->setTerminalState();
		return 1.0;
	}

	//reached the minimum position to the left?
	if (position == s_p->getProperties("position").getMin())
	{
		//in Sutton's description the experiment would now be terminated.
		//In the Degris' the experiment is only terminated at the right side of the world.
		//(see https://hal.inria.fr/hal-00764281/document)

		//SimionApp::get()->pExperiment->setTerminalState();
		return -1.0;
	}
	return -1.0;
}

double MountainCarReward::getMin() { return -1.0; }

double MountainCarReward::getMax() { return 1.0; }