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

#include "world.h"
#include "pitchcontrol.h"
#include "../../Common/named-var-set.h"
#include "setpoint.h"
#include "../reward.h"
#include "../experiment.h"
#include "../config.h"
#include "../app.h"

PitchControl::PitchControl(ConfigNode* pConfigNode)
{
	METADATA("World", "Pitch-control");
	m_sSetpointPitch = addStateVariable("setpoint-pitch","rad", -3.14159265, 3.14159265);
	m_sAttackAngle = addStateVariable("attack-angle","rad", -3.14159265, 3.14159265);
	m_sPitch= addStateVariable("pitch","rad", -3.14159265, 3.14159265);
	m_sPitchRate = addStateVariable("pitch-rate","rad/s",-0.1,0.1);
	m_sControlDeviation = addStateVariable("control-deviation","rad",-6.5,6.5);

	m_aPitch = addActionVariable("pitch","rad",-1.4,1.4);

	FILE_PATH_PARAM filename= FILE_PATH_PARAM(pConfigNode, "Set-Point-File","The setpoint file", "../config/world/pitch-control/setpoint.txt");
	m_pSetpoint = new FileSetPoint(filename.get());

	m_pRewardFunction = new RewardFunction();
	m_pRewardFunction->addRewardComponent(new ToleranceRegionReward("control-deviation", 0.02, 1.0));
	m_pRewardFunction->initialize();
}

PitchControl::~PitchControl()
{
	delete m_pSetpoint;
}

void PitchControl::reset(State *s)
{
	double u;

	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
		//setpoint file in case we're evaluating
		s->set(m_sSetpointPitch,m_pSetpoint->getPointSet(0.0));
	else
	{
		//random point in [-0.5,0.5]
		u= ((double)(rand()%10000))/ 10000.0;
		s->set(m_sSetpointPitch, (2 * u - 0.5)*0.5);
	}
	s->set(m_sAttackAngle,0.0);
	s->set(m_sPitch,0.0);
	s->set(m_sPitchRate,0.0);
	s->set(m_sControlDeviation,m_pSetpoint->getPointSet(0.0));
}

void PitchControl::executeAction(State *s, const Action *a, double dt)
{
	double setpoint_pitch;
	
	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		setpoint_pitch = m_pSetpoint->getPointSet(SimionApp::get()->pWorld->getEpisodeSimTime());
		s->set(m_sSetpointPitch, setpoint_pitch);
	}
	else
		setpoint_pitch = s->get(m_sSetpointPitch);

	double angle_attack= s->get(m_sAttackAngle);
	double pitch_rate= s->get(m_sPitchRate);
	double u= a->get(m_aPitch);

	double angle_attack_dot= -0.313*angle_attack + 56.7*pitch_rate + 0.232*u;
	double pitch_rate_dot= -0.0139*angle_attack -0.426*pitch_rate + 0.0203*u;
	double pitch_angle_dot= 56.7*pitch_rate;

	s->set(m_sAttackAngle,angle_attack + angle_attack_dot*dt);
	s->set(m_sPitchRate,pitch_rate + pitch_rate_dot*dt);
	s->set(m_sPitch, s->get(m_sPitch) + pitch_angle_dot*dt);
	s->set(m_sControlDeviation,setpoint_pitch - s->get(m_sPitch));
}