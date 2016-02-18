#include "stdafx.h"
#include "world-underwatervehicle.h"
#include "states-and-actions.h"
#include "setpoint.h"
#include "xml-parameters.h"


#define NUM_STATE_VARIABLES 3


CUnderwaterVehicle::CUnderwaterVehicle(tinyxml2::XMLElement* pParameters)
: CDynamicModel(XMLParameters::getConstString(pParameters->FirstChildElement("WORLD-DEFINITION")))
{
	/*
	m_pStateDescriptor= new CState(3);
	m_pStateDescriptor->setProperties(0,"v-setpoint",-5.0,5.0);
	m_pStateDescriptor->setProperties(1,"v",-5.0,5.0);
	m_pStateDescriptor->setProperties(2,"v-deviation",-10.0,10.0);

	m_pActionDescriptor= new CAction(1);
	m_pActionDescriptor->setProperties(0,"u-thrust",-30.0,30.0);*/

	m_pSetpoint= new CFileSetPoint(pParameters->FirstChildElement("SET_POINT_FILE")->GetText());
}

CUnderwaterVehicle::~CUnderwaterVehicle()
{
	delete m_pSetpoint;
}



void CUnderwaterVehicle::reset(CState *s)
{
	s->setValue("v-setpoint",m_pSetpoint->getPointSet(0.0));
	s->setValue("v-deviation",m_pSetpoint->getPointSet(0.0));
	s->setValue("v",0.0);
}

void CUnderwaterVehicle::executeAction(CState *s,CAction *a,double dt)
{
	double newSetpoint= m_pSetpoint->getPointSet(CWorld::getT());
	double v= s->getValue("v");
	double u= a->getValue(0); //thrust
	double dot_v= (u*(-0.5*tanh((fabs((1.2+0.2*sin(fabs(v)))*v*fabs(v) - u) -30.0)*0.1) + 0.5) 
		- (1.2+0.2*sin(fabs(v)))*v*fabs(v))	/(3.0+1.5*sin(fabs(v)));
	double newV= v + dot_v*dt;

	s->setValue("v",newV);
	s->setValue("v-setpoint",newSetpoint);
	s->setValue("v-deviation",newSetpoint-newV);
}
