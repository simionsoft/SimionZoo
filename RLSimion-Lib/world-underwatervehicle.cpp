#include "stdafx.h"
#include "world-underwatervehicle.h"
#include "named-var-set.h"
#include "setpoint.h"
#include "xml-parameters.h"
#include "world.h"
#include "globals.h"

#define NUM_STATE_VARIABLES 3


CUnderwaterVehicle::CUnderwaterVehicle(tinyxml2::XMLElement* pParameters)
: CDynamicModel(XMLUtils::getConstString(pParameters, "World-Definition"))
{
	CState *pStateDescriptor = getStateDescriptor();
	m_sVSetpoint = pStateDescriptor->getVarIndex("v-setpoint");
	m_sV = pStateDescriptor->getVarIndex("v");
	m_sVDeviation = pStateDescriptor->getVarIndex("v-deviation");

	CAction *pActionDescriptor = getActionDescriptor();
	m_aUThrust = pActionDescriptor->getVarIndex("u-thrust");

	m_pSetpoint= new CFileSetPoint(pParameters->FirstChildElement("Set-Point-File")->GetText());
}

CUnderwaterVehicle::~CUnderwaterVehicle()
{
	delete m_pSetpoint;
}



void CUnderwaterVehicle::reset(CState *s)
{
	s->setValue(m_sVSetpoint,m_pSetpoint->getPointSet(0.0));
	s->setValue(m_sVDeviation,m_pSetpoint->getPointSet(0.0));
	s->setValue(m_sV,0.0);
}

void CUnderwaterVehicle::executeAction(CState *s,CAction *a,double dt)
{
	double newSetpoint = m_pSetpoint->getPointSet(RLSimion::g_pWorld->getT());
	double v= s->getValue(m_sV);
	double u= a->getValue(m_aUThrust); //thrust
	double dot_v= (u*(-0.5*tanh((fabs((1.2+0.2*sin(fabs(v)))*v*fabs(v) - u) -30.0)*0.1) + 0.5) 
		- (1.2+0.2*sin(fabs(v)))*v*fabs(v))	/(3.0+1.5*sin(fabs(v)));
	double newV= v + dot_v*dt;

	s->setValue(m_sV,newV);
	s->setValue(m_sVSetpoint,newSetpoint);
	s->setValue(m_sVDeviation,newSetpoint-newV);
}
