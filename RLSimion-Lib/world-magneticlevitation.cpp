#include "stdafx.h"
#include "world.h"
#include "world-magneticlevitation.h"
#include "states-and-actions.h"
#include "parameters.h"
#include "parameter.h"
#include "setpoint.h"
#include "globals.h"
#include "experiment.h"

CMagneticLevitation::CMagneticLevitation(CParameters* pParameters)
{
	m_pStateDescriptor= new CState(5);
	
	m_pStateDescriptor->setProperties(0,"position-setpoint",0.0,0.013);
	m_pStateDescriptor->setProperties(1,"position",0.0,0.013);
	m_pStateDescriptor->setProperties(2,"velocity",-100.0,100.0);//??????????????
	m_pStateDescriptor->setProperties(3,"current",-100,100);//?????????????
	m_pStateDescriptor->setProperties(4,"position-deviation",-0.13,0.13);

	m_pActionDescriptor= new CAction(1);
	m_pActionDescriptor->setProperties(0,"voltage",-60,60);

	m_pEvalSetPoint= new CFileSetPoint(pParameters->getParameter("EVALUATION_SET_POINT_FILE")->getStringPtr());
	m_pLearnSetPoint= new CFixedStepSizeSetPoint(0.32,0.0, 0.013);
}

CMagneticLevitation::~CMagneticLevitation()
{
	delete m_pEvalSetPoint;
	delete m_pLearnSetPoint;
}

void CMagneticLevitation::reset(CState *s)
{
	CSetPoint *pSetPoint;
	if (g_pExperiment->isEvaluationEpisode())
		pSetPoint= m_pEvalSetPoint;
	else
		pSetPoint= m_pLearnSetPoint;

	double setpoint= pSetPoint->getPointSet(0.0);
	s->setValue("position-setpoint",setpoint);
	s->setValue("position",0.013);
	s->setValue("velocity",0.0);
	s->setValue("current",2.1285389233672252);//0.0);
	s->setValue("position-deviation",0.013-setpoint);


	//initialization procedure: 0.5 seconds with 15V
	/*CAction *a= g_pWorld->getActionDescriptor();
	a->setValue("voltage",15.0);
	double t= 0.0;
	double dt= 0.01;
	for (t= 0.0; t<0.5; t+= dt)
		executeAction(s,a,dt);*/
}

#define M 0.8
#define R 11.68
#define x_inf 0.007
#define L_inf 0.8052
#define xi 0.001599
#define G 9.8

void CMagneticLevitation::executeAction(CState *s, CAction *a, double dt)
{
	double d= s->getValue("position");
	double d_dot= s->getValue("velocity");
	double I= s->getValue("current");
	double setpoint;
	
	if (g_pExperiment->isEvaluationEpisode())
		setpoint= m_pEvalSetPoint->getPointSet(CWorld::getT());
	else
		setpoint= m_pLearnSetPoint->getPointSet(CWorld::getT());



	double d_dot_dot= G - (xi*I*I/(2*M*pow(x_inf+d,2.0)));
	double I_dot_i= I*(xi*d_dot-R*pow(x_inf+d,2.0))/(xi*(x_inf+d) + L_inf*pow(x_inf+d,2.0));
	double I_dot_ii= (x_inf+d)/(xi + L_inf*(x_inf+d));
	double I_dot= I_dot_i + a->getValue("voltage")*I_dot_ii;

	s->setValue("position", d + d_dot*dt);
	s->setValue("velocity", d_dot + d_dot_dot*dt);
	s->setValue("current", I + I_dot*dt);

	s->setValue("position-setpoint",setpoint);
	s->setValue("position-deviation",s->getValue("position")-setpoint);
}