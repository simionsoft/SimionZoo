#include "stdafx.h"
#include "world.h"
#include "world-pitchcontrol.h"
#include "named-var-set.h"
#include "setpoint.h"
#include "globals.h"
#include "reward.h"
#include "experiment.h"
#include "config.h"
#include "app.h"

CLASS_CONSTRUCTOR(CPitchControl,const char* worldDefinition)
: CDynamicModel(worldDefinition)
{
	CState *pStateDescriptor = getStateDescriptor();
	m_sSetpointPitch = pStateDescriptor->getVarIndex("setpoint-pitch");
	m_sAttackAngle = pStateDescriptor->getVarIndex("attack-angle");
	m_sPitch= pStateDescriptor->getVarIndex("pitch");
	m_sPitchRate = pStateDescriptor->getVarIndex("pitch-rate");
	m_sControlDeviation = pStateDescriptor->getVarIndex("control-deviation");

	CAction *pActionDescriptor = getActionDescriptor();
	m_aPitch = pActionDescriptor->getVarIndex("pitch");

	const char* filename;
	FILE_PATH_VALUE(filename, "Set-Point-File", "../config/world/pitch-control/setpoint.txt","The setpoint file");
	m_pSetpoint = new CFileSetPoint(filename);

	m_pRewardFunction = new CRewardFunction();
	m_pRewardFunction->addRewardComponent(new CToleranceRegionReward("control-deviation", 0.02, 1.0));
	m_pRewardFunction->initialize();
	
	END_CLASS();
}

CPitchControl::~CPitchControl()
{
	delete m_pSetpoint;
}

void CPitchControl::reset(CState *s)
{
	double u;

	if (CApp::get()->pExperiment->isEvaluationEpisode())
		//setpoint file in case we're evaluating
		s->setValue(m_sSetpointPitch,m_pSetpoint->getPointSet(0.0));
	else
	{
		//random point in [-0.5,0.5]
		u= ((double)(rand()%10000))/ 10000.0;
		s->setValue(m_sSetpointPitch, (2 * u - 0.5)*0.5);
	}
	s->setValue(m_sAttackAngle,0.0);
	s->setValue(m_sPitch,0.0);
	s->setValue(m_sPitchRate,0.0);
	s->setValue(m_sControlDeviation,m_pSetpoint->getPointSet(0.0));
}

void CPitchControl::executeAction(CState *s, const CAction *a, double dt)
{
	double setpoint_pitch;
	
	if (CApp::get()->pExperiment->isEvaluationEpisode())
	{
		setpoint_pitch = m_pSetpoint->getPointSet(CApp::get()->pWorld->getT());
		s->setValue(m_sSetpointPitch, setpoint_pitch);
	}
	else
		setpoint_pitch = s->getValue(m_sSetpointPitch);

	double angle_attack= s->getValue(m_sAttackAngle);
	double pitch_rate= s->getValue(m_sPitchRate);
	double u= a->getValue(m_aPitch);

	double angle_attack_dot= -0.313*angle_attack + 56.7*pitch_rate + 0.232*u;
	double pitch_rate_dot= -0.0139*angle_attack -0.426*pitch_rate + 0.0203*u;
	double pitch_angle_dot= 56.7*pitch_rate;

	s->setValue(m_sAttackAngle,angle_attack + angle_attack_dot*dt);
	s->setValue(m_sPitchRate,pitch_rate + pitch_rate_dot*dt);
	s->setValue(m_sPitch, s->getValue(m_sPitch) + pitch_angle_dot*dt);
	s->setValue(m_sControlDeviation,setpoint_pitch - s->getValue(m_sPitch));
}