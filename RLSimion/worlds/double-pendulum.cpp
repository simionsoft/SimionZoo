#include "double-pendulum.h"
#include "../simgod.h"
#include "../experiment.h"
#include "../named-var-set.h"
#include "../config.h"
#include "../reward.h"
#include "../app.h"
#include "../noise.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define G 9.8
#define M_1 1.0
#define M_2 1.0
#define S_1 1.0
#define S_1 1.0
#define MAX_TORQUE 8.5


DoublePendulum::DoublePendulum(ConfigNode* pConfigNode)
{
	METADATA("World", "Double-pendulum");
	m_sTheta1 = addStateVariable("theta1", "rad", -3.14159265358979323846, 3.14159265358979323846, true); // M_PI
	m_sTheta1Dot = addStateVariable("theta1dot", "rad/s", -10.0, 10.0);
	m_sTheta2 = addStateVariable("theta2", "rad", -3.14159265358979323846, 3.14159265358979323846, true); // M_PI
	m_sTheta1Dot = addStateVariable("theta2dot", "rad/s", -10.0, 10.0);
	m_sJointPosX = addStateVariable("joint-pos-x", "m", -1.0, 1.0);
	m_sJointPosY = addStateVariable("joint-pos-y", "m", -1.0, 1.0);
	
	m_aTorque1 = addActionVariable("torque1", "Nm", -8.5, 8.5);
	m_aTorque2 = addActionVariable("torque2", "Nm", -8.5, 8.5);

	//the reward function
	m_pRewardFunction->addRewardComponent(new DoublePendulumReward());
	m_pRewardFunction->initialize();
}

DoublePendulum::~DoublePendulum()
{

}

void DoublePendulum::reset(State *s)
{
	//start hanging motionless
	s->set(m_sTheta1, 0.0);
	s->set(m_sTheta1Dot, 0.0);
	s->set(m_sTheta2, 0.0);
	s->set(m_sTheta2Dot, 0.0);
	s->set(m_sJointPosX, sin(s->get(m_sTheta1)));
	s->set(m_sJointPosY, cos(s->get(m_sTheta1)));
}

void DoublePendulum::executeAction(State *s, const Action *a, double dt)
{


	s->set(m_sTheta1, 0.0);
	s->set(m_sTheta1Dot, 0.0);
	s->set(m_sTheta2, 0.0);
	s->set(m_sTheta2Dot, 0.0);
	s->set(m_sJointPosX, sin(s->get(m_sTheta1)));
	s->set(m_sJointPosY, cos(s->get(m_sTheta1)));
}

double DoublePendulumReward::getReward(const State* s, const Action* a, const State* s_p)
{
	double angle = s_p->get("angle");

	if (abs(angle) < 0.05)
		//measure the time within the target angle range
		m_timeInGoal += 1.0 / (2.0 / SimionApp::get()->pWorld->getDT());
	else
		m_timeInGoal = 0.0;

	if (m_timeInGoal >= 1.0)
		SimionApp::get()->pExperiment->setTerminalState();

	return cos(angle);
}

double DoublePendulumReward::getMin() { return -1.0; }

double DoublePendulumReward::getMax() { return 1.0; }