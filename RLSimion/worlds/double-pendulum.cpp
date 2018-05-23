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
#include <algorithm>

#define G 9.8
#define M_1 1.0
#define M_2 1.0
#define S_1 1.0
#define S_1 1.0
#define MAX_TORQUE 8.5


DoublePendulum::DoublePendulum(ConfigNode* pConfigNode)
{
	METADATA("World", "Double-pendulum");

	m_sTheta1 = addStateVariable("theta_1", "rad", -3.14159265358979323846, 3.14159265358979323846, true); // M_PI
	m_sTheta1Dot = addStateVariable("theta_1-dot", "rad/s", -10.0, 10.0);
	m_sTheta2 = addStateVariable("theta_2", "rad", -3.14159265358979323846, 3.14159265358979323846, true); // M_PI
	m_sTheta2Dot = addStateVariable("theta_2-dot", "rad/s", -10.0, 10.0);
	
	m_aTorque1 = addActionVariable("torque_1", "Nm", -8.5, 8.5);
	m_aTorque2 = addActionVariable("torque_2", "Nm", -8.5, 8.5);

	addConstant("s_1", 1.0);
	addConstant("s_2", 1.0);
	addConstant("m_1", 1.0);
	addConstant("m_2", 1.0);
	addConstant("g", 9.8);

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
}

void DoublePendulum::executeAction(State *s, const Action *a, double dt)
{
	//Equations from:
	//https://scholarworks.umass.edu/cgi/viewcontent.cgi?referer=https://www.google.com/&httpsredir=1&article=1130&context=cs_faculty_pubs
	double s_1 = getConstant("s_1");
	double s_2 = getConstant("s_2");
	double m_1 = getConstant("m_1");
	double m_2 = getConstant("m_2");
	double g = getConstant("g");
	double theta_1_dot = s->get(m_sTheta1Dot);
	double theta_2_dot = s->get(m_sTheta2Dot);
	double theta_1 = s->get(m_sTheta1);
	double theta_2 = s->get(m_sTheta2);
	double theta_dif = theta_1 - theta_2;
	double theta_1_dot_dot = (-s_1 * m_2*theta_1_dot*theta_1_dot * sin(theta_dif)*cos(theta_dif)
		- m_2 * s_2*theta_2_dot*theta_2_dot*sin(theta_dif)
		+ g * m_2*sin(theta_2)*cos(theta_dif)
		- g * (m_1 + m_2)*sin(theta_1) + a->get(m_aTorque1))
		/ (s_1*(m_1+m_2*sin(theta_dif)*sin(theta_dif)));
	double theta_2_dot_dot = (-s_1 * theta_1_dot_dot*cos(theta_dif)
		+ s_1 * theta_1_dot*theta_1_dot*sin(theta_dif
			- g * sin(theta_2) + a->get(m_aTorque2) / m_2)) / s_2;

	s->set(m_sTheta1, theta_1 + theta_1_dot * dt);
	s->set(m_sTheta1Dot, theta_1_dot + theta_1_dot_dot * dt);
	s->set(m_sTheta2, theta_2 + theta_2_dot * dt);
	s->set(m_sTheta2Dot, theta_2_dot + theta_2_dot_dot * dt);
}

double DoublePendulumReward::getReward(const State* s, const Action* a, const State* s_p)
{
	//https://scholarworks.umass.edu/cgi/viewcontent.cgi?referer=https://www.google.com/&httpsredir=1&article=1130&context=cs_faculty_pubs
	double theta_1 = s->get("theta_1");
	double theta_2 = s->get("theta_2");
	double dist1 = std::min(abs(3.1415 - theta_1), abs(-3.1415 - theta_1));
	double dist2 = std::min(abs(3.1415 - theta_1), abs(-3.1415 - theta_1));
	double tolerance = 0.75;
	if (dist1 < tolerance && dist2 < tolerance)
		return 0.0;
	return -1;
}

double DoublePendulumReward::getMin() { return -1.0; }

double DoublePendulumReward::getMax() { return 0.0; }