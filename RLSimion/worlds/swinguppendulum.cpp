#include "swinguppendulum.h"
#include "../named-var-set.h"
#include "../config.h"
#include "../reward.h"
#include "../app.h"
#include "../noise.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define g 9.8
#define m 1
#define l 1
#define mu 0.01


SwingupPendulum::SwingupPendulum(ConfigNode* pConfigNode)
{
	METADATA("World", "Swing-up-pendulum");
	m_sAngle = addStateVariable("angle", "rad", -3.14, 3.14, true); // M_PI
	m_sAngularVelocity = addStateVariable("angular-velocity", "rad/s", -25, 25); //25 * M_PI
	m_sNormalizedTimeInTargetPosition = addStateVariable("normalized-time-in-target-position", "", 0.0, 1.0);
	
	m_aTorque = addActionVariable("torque", "Nm", -2.0, 2.0); //maxTorque=2.0

	//the reward function
	m_pRewardFunction->addRewardComponent(new SwingupPendulumReward());
	m_pRewardFunction->initialize();
}

SwingupPendulum::~SwingupPendulum()
{

}

void SwingupPendulum::reset(State *s)
{
	//fix theta € [-pi, pi]
	//omega = 0
	s->set(m_sAngle, M_PI); //getRandomValue() * 2 * M_PI - M_PI
	s->set(m_sAngularVelocity, 0.0);
	s->set(m_sNormalizedTimeInTargetPosition, 0.0);
}

void SwingupPendulum::executeAction(State *s, const Action *a, double dt)
{
	double angle = s->get(m_sAngle);
	double angularVelocity = s->get(m_sAngularVelocity);
	double torque = a->get(m_aTorque);
	double normalizedTimeInTargetPosition = s->get(m_sNormalizedTimeInTargetPosition);

	double old_angle = angle;
	
	double angularAcceleration = (-mu*angularVelocity + m*g*l*sin(angle) + torque) / (m*l*l);
	angle += dt * angularVelocity;
	angularVelocity += dt * angularAcceleration;

	if (angle < -3.14)
	{
		angle += 6.28;
	}
	else if (angle > 3.14)
	{
		angle -= 6.28;
	}

	if (abs(old_angle) < 0.05)
	{
		if (abs(angle) < 0.05)
		{
			//stay there for 2 second
			normalizedTimeInTargetPosition += 1.0 / (2.0 / dt);
		}
		else
		{
			normalizedTimeInTargetPosition = 0.0;
		}
	}

	if (angularVelocity < -25)
	{
		angularVelocity = -25;
	}
	else if (angularVelocity > 25)
	{
		angularVelocity = 25;
	}

	s->set(m_sAngle, angle);
	s->set(m_sAngularVelocity, angularVelocity);
	s->set(m_sNormalizedTimeInTargetPosition, normalizedTimeInTargetPosition);
}

double SwingupPendulumReward::getReward(const State* s, const Action* a, const State* s_p)
{
	double angel = s_p->get("angle");
	double normalizedTimeInTargetPosition = s->get("normalized-time-in-target-position");
	
	if (normalizedTimeInTargetPosition >= 1.0)
	{
		SimionApp::get()->pExperiment->setTerminalState();
	}
	

	return cos(angel);
}

double SwingupPendulumReward::getMin() { return -1.0; }

double SwingupPendulumReward::getMax() { return 1.0; }