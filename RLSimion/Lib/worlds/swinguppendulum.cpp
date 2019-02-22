/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "swinguppendulum.h"
#include "../simgod.h"
#include "../experiment.h"
#include "../../Common/named-var-set.h"
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
	m_sAngle = addStateVariable("angle", "rad", -3.14159265358979323846, 3.14159265358979323846, true); // M_PI
	m_sAngularVelocity = addStateVariable("angular-velocity", "rad/s", -25, 25); //25 * M_PI
	
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
	//fix theta � [-pi, pi]
	//omega = 0
	s->set(m_sAngle, M_PI);
	s->set(m_sAngularVelocity, 0.0);
}

void SwingupPendulum::executeAction(State *s, const Action *a, double dt)
{
	double angle = s->get(m_sAngle);
	double oldAngle = angle;
	double angularVelocity = s->get(m_sAngularVelocity);
	double torque = a->get(m_aTorque);
	
	double angularAcceleration;
	//to make it stable when torque is zero for a long period of time
	if (torque!=0.0 || angularVelocity!=0.0 || angle!=M_PI)
		angularAcceleration= (-mu * angularVelocity + m * g*l*sin(angle) + torque) / (m*l*l);
	else angularAcceleration = 0.0;
	angle += dt * angularVelocity;
	angularVelocity += dt * angularAcceleration;

	s->set(m_sAngle, angle);
	s->set(m_sAngularVelocity, angularVelocity);
}

double SwingupPendulumReward::getReward(const State* s, const Action* a, const State* s_p)
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

double SwingupPendulumReward::getMin() { return -1.0; }

double SwingupPendulumReward::getMax() { return 1.0; }