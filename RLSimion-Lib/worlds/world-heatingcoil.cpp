#include "../stdafx.h"
#include "world.h"
#include "heatingcoil.h"
#include "../named-var-set.h"
#include "../parameters.h"
#include "setpoint.h"

CHeatingCoil::CHeatingCoil(CParameters* pParameters)
{
	addStateVariable("setpoint-pitch","rad",-3.14159265,3.14159265);
	addStateVariable("pitch","rad",-3.14159265,3.14159265);
	addStateVariable("attack-angle","rad",-3.14159265,3.14159265);
	addStateVariable("pitch-rate","rad/s",-0.1,0.1);
	addStateVariable("control-deviation","rad",-6.5,6.5);

	addActionVariable("pitch","rad",-1.4,1.4);

	m_pSetpoint= new CFileSetPoint("pitch-control.txt");
}

CHeatingCoil::~CHeatingCoil()
{
	delete m_pSetpoint;
}

void CPitchControl::reset(CState *s)
{
	s->setValue("setpoint-pitch",m_pSetpoint->getPointSet(0.0));
	s->setValue("attack-angle",0.0);
	s->setValue("pitch",0.0);
	s->setValue("pitch-rate",0.0);
	s->setValue("control-deviation",m_pSetpoint->getPointSet(0.0));
}

void CPitchControl::executeAction(CState *s, CAction *a, double dt)
{
	double setpoint_pitch= m_pSetpoint->getPointSet(CWorld::m_t);
	s->setValue("setpoint-pitch",setpoint_pitch);

	double angle_attack= s->getValue("attack-angle");
	double pitch_rate= s->getValue("pitch-rate");
	double u= a->getValue("pitch");

	double angle_attack_dot= -0.313*angle_attack + 56.7*pitch_rate + 0.232*u;
	double pitch_rate_dot= -0.0139*angle_attack -0.426*pitch_rate + 0.0203*u;
	double pitch_angle_dot= 56.7*pitch_rate;

	s->setValue("attack-angle",angle_attack + angle_attack_dot*dt);
	s->setValue("pitch-rate",pitch_rate + pitch_rate_dot*dt);
	s->setValue("pitch", s->getValue("pitch") + pitch_angle_dot*dt);
	s->setValue("control-deviation",s->getValue("pitch")-setpoint_pitch);
}