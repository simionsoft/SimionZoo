#include "world.h"
#include "magneticlevitation.h"
#include "../named-var-set.h"
#include "setpoint.h"
#include "../experiment.h"
#include "../config.h"
#include "../app.h"
#include "../reward.h"


MagneticLevitation::MagneticLevitation(ConfigNode* pConfigNode)
{
	METADATA("World", "Magnetic-levitation");
	m_sPositionSetpoint= addStateVariable("position-setpoint","m",0.0,0.013);
	m_sPosition= addStateVariable("position","m",0.0,0.013);
	m_sVelocity= addStateVariable("velocity","m/s",-100.0,100.0);//??????????????
	m_sCurrent= addStateVariable("current","A",-100,100);//?????????????
	m_sPositionDeviation = addStateVariable("position-deviation","m",-0.13,0.13);

	addActionVariable("voltage","V",-60,60);


	FILE_PATH_PARAM filename= FILE_PATH_PARAM(pConfigNode, "Evaluation-Set-Point"
		,"The setpoint file", "../config/world/magnetic-levitation/setpoint.txt");
	m_pEvalSetPoint = new FileSetPoint(filename.get());

	m_pLearnSetPoint= new FixedStepSizeSetPoint(0.32,0.0, 0.013);

	m_pRewardFunction->addRewardComponent(new ToleranceRegionReward("position-deviation", 0.001, 1.0));
	m_pRewardFunction->initialize();
}

MagneticLevitation::~MagneticLevitation()
{
	delete m_pEvalSetPoint;
	delete m_pLearnSetPoint;
}

void MagneticLevitation::reset(State *s)
{
	SetPoint *pSetPoint;
	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
		pSetPoint= m_pEvalSetPoint;
	else
		pSetPoint= m_pLearnSetPoint;

	double setpoint= pSetPoint->getPointSet(0.0);
	s->set(m_sPositionSetpoint,setpoint);
	s->set(m_sPosition,0.013);
	s->set(m_sVelocity,0.0);
	s->set(m_sCurrent,2.1285389233672252);//0.0);
	s->set(m_sPositionDeviation,0.013-setpoint);


	//initialization procedure: 0.5 seconds with 15V
	/*Action *a= SimionApp::getSample()->pWorld->getActionDescriptor();
	a->set("voltage",15.0);
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

void MagneticLevitation::executeAction(State *s, Action *a, double dt)
{
	double d= s->get(m_sPosition);
	double d_dot= s->get(m_sVelocity);
	double I= s->get(m_sCurrent);
	double setpoint;
	
	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
		setpoint = m_pEvalSetPoint->getPointSet(SimionApp::get()->pWorld->getEpisodeSimTime());
	else
		setpoint = m_pLearnSetPoint->getPointSet(SimionApp::get()->pWorld->getEpisodeSimTime());



	double d_dot_dot= G - (xi*I*I/(2*M*pow(x_inf+d,2.0)));
	double I_dot_i= I*(xi*d_dot-R*pow(x_inf+d,2.0))/(xi*(x_inf+d) + L_inf*pow(x_inf+d,2.0));
	double I_dot_ii= (x_inf+d)/(xi + L_inf*(x_inf+d));
	double I_dot= I_dot_i + a->get(m_aVoltage)*I_dot_ii;

	s->set(m_sPosition, d + d_dot*dt);
	s->set(m_sVelocity, d_dot + d_dot_dot*dt);
	s->set(m_sCurrent, I + I_dot*dt);

	s->set(m_sPositionSetpoint,setpoint);
	s->set(m_sPositionDeviation,s->get(m_sPosition)-setpoint);
}