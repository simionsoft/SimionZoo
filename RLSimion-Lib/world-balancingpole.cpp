#include "stdafx.h"
#include "world-balancingpole.h"
#include "named-var-set.h"
#include "globals.h"
#include "parameters.h"
#include "reward.h"
#include "app.h"
#include "noise.h"

CLASS_CONSTRUCTOR(CBalancingPole, const char* worldDefinition)
: CDynamicModel(worldDefinition)
{
	CState *pStateDescriptor = getStateDescriptor();
	m_sX = pStateDescriptor->getVarIndex("x");
	m_sX_dot = pStateDescriptor->getVarIndex("x_dot");
	m_sTheta = pStateDescriptor->getVarIndex("theta");
	m_sTheta_dot = pStateDescriptor->getVarIndex("theta_dot");

	CAction *pActionDescriptor = getActionDescriptor();
	m_aPitch = pActionDescriptor->getVarIndex("force");

	GRAVITY= getConstant("GRAVITY");
	MASSCART= getConstant("MASSCART");
	MASSPOLE = getConstant("MASSPOLE");
	TOTAL_MASS= getConstant("TOTAL_MASS");
	LENGTH= getConstant("LENGTH");
	POLEMASS_LENGTH= getConstant("POLEMASS_LENGTH");

	//the reward function
	m_pRewardFunction->addRewardComponent(new CBalancingPoleReward());
	m_pRewardFunction->initialize();
	
	END_CLASS();
}

CBalancingPole::~CBalancingPole()
{

}

void CBalancingPole::reset(CState *s)
{

	if (CApp::get()->Experiment.isEvaluationEpisode())
	{
		//fixed setting in evaluation episodes
		s->setValue(m_sTheta, -0.1);
		s->setValue(m_sTheta_dot, -0.01);
		s->setValue(m_sX, 0.0);
		s->setValue(m_sX_dot, 0.0);
	}
	else
	{
		//random setting in training episodes
		s->setValue(m_sTheta, -0.2 + getRandomValue()*0.4);
		s->setValue(m_sTheta_dot, -0.05 + getRandomValue()*0.1);
		s->setValue(m_sX, -0.5 + getRandomValue());
		s->setValue(m_sX_dot, -0.1 + getRandomValue()*0.2);
	}
}

//#define GRAVITY 9.8
//#define MASSCART 1.0
//#define MASSPOLE 0.1
//#define TOTAL_MASS (MASSPOLE + MASSCART)
//#define LENGTH 0.5		  /* actually half the pole's length */
//#define POLEMASS_LENGTH (MASSPOLE * LENGTH)
//#define FORCE_MAG 10.0
//#define TAU 0.02		  /* seconds between state updates */
#define FOURTHIRDS 1.3333333333333

void CBalancingPole::executeAction(CState *s, const CAction *a, double dt)
{
	double force = a->getValue(m_aPitch);
	double theta = s->getValue(m_sTheta);
	double theta_dot = s->getValue(m_sTheta_dot);
	double x = s->getValue(m_sX);
	double x_dot = s->getValue(m_sX_dot);
	double costheta = cos(theta);
	double sintheta = sin(theta);

	double temp = (force + POLEMASS_LENGTH * theta_dot * theta_dot * sintheta)
		/ TOTAL_MASS;

	double thetaacc = (GRAVITY * sintheta - costheta* temp)
		/ (LENGTH * (FOURTHIRDS - MASSPOLE * costheta * costheta
		/ TOTAL_MASS));

	double xacc = temp - POLEMASS_LENGTH * thetaacc* costheta / TOTAL_MASS;

	/*** Update the four state variables, using Euler's method. ***/
	s->setValue(m_sX, x + x_dot*dt);
	s->setValue(m_sX_dot, x_dot + xacc*dt);
	s->setValue(m_sTheta, theta + theta_dot*dt);
	s->setValue(m_sTheta_dot, theta_dot + thetaacc*dt);
}

#define twelve_degrees 0.2094384
double CBalancingPoleReward::getReward(const CState* s, const CAction* a, const CState* s_p)
{
	bool bEval = CApp::get()->Experiment.isEvaluationEpisode();
	int step = CApp::get()->Experiment.getStep();
	double theta = s_p->getValue("theta");
	double x = s_p->getValue("x");

	if (x < -2.4 ||
		x > 2.4 ||
		theta < -twelve_degrees ||
		theta > twelve_degrees)
	{
		CApp::get()->Experiment.setTerminalState();
		return -1.0;
	}

	return 0.0;
}