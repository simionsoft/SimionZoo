#include "stdafx.h"
#include "world.h"
#include "world-pitchcontrol.h"
#include "named-var-set.h"
#include "setpoint.h"
#include "reward.h"
#include "experiment.h"
#include "config.h"
#include "app.h"

CPitchControl::CPitchControl(CConfigNode* pConfigNode)
{
	METADATA("World", "Pitch-control");
	m_sSetpointPitch = addStateVariable("setpoint-pitch","rad", -3.14159265, 3.14159265);
	m_sAttackAngle = addStateVariable("attack-angle","rad", -3.14159265, 3.14159265);
	m_sPitch= addStateVariable("pitch","rad", -3.14159265, 3.14159265);
	m_sPitchRate = addStateVariable("pitch-rate","rad/s",-0.1,0.1);
	m_sControlDeviation = addStateVariable("control-deviation","rad",-6.5,6.5);

	m_aPitch = addActionVariable("pitch","rad",-1.4,1.4);

	FILE_PATH_PARAM filename= FILE_PATH_PARAM(pConfigNode, "Set-Point-File","The setpoint file", "../config/world/pitch-control/setpoint.txt");
	m_pSetpoint = new CFileSetPoint(filename.get());

	m_pRewardFunction = new CRewardFunction();
	m_pRewardFunction->addRewardComponent(new CToleranceRegionReward("control-deviation", 0.02, 1.0));
	m_pRewardFunction->initialize();
}

CPitchControl::~CPitchControl()
{
	delete m_pSetpoint;
}

void CPitchControl::reset(CState *s)
{
	double u;

	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
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

void CPitchControl::executeAction(CState *s, const CAction *a, double dt)
{
	double setpoint_pitch;
	
	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		setpoint_pitch = m_pSetpoint->getPointSet(CSimionApp::get()->pWorld->getEpisodeSimTime());
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