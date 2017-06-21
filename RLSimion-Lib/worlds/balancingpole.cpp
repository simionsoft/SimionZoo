#include "../stdafx.h"
#include "balancingpole.h"
#include "../named-var-set.h"
#include "../config.h"
#include "../reward.h"
#include "../app.h"
#include "../noise.h"


CBalancingPole::CBalancingPole(CConfigNode* pConfigNode)
{
	METADATA("World", "Balancing-pole");
	m_sX = addStateVariable("x", "m", -3.0, 3.0);
	m_sX_dot = addStateVariable("x_dot","m/s",-1.0,1.0);
	m_sTheta = addStateVariable("theta","rad",-0.22,0.22);
	m_sTheta_dot = addStateVariable("theta_dot","rad/s",-1.0,1.0);

	m_aPitch = addActionVariable("force","N",-10.0,10.0);

	GRAVITY = 9.8;
	MASSCART = 1.0;
	MASSPOLE = 0.1;
	TOTAL_MASS = 1.1;
	LENGTH = 0.5;
	POLEMASS_LENGTH = 0.05;

	//the reward function
	m_pRewardFunction->addRewardComponent(new CBalancingPoleReward());
	m_pRewardFunction->initialize();
}


void CBalancingPole::reset(CState *s)
{

	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//fixed setting in evaluation episodes
		s->set(m_sTheta, -0.1);
		s->set(m_sTheta_dot, -0.01);
		s->set(m_sX, 0.0);
		s->set(m_sX_dot, 0.0);
	}
	else
	{
		//random setting in training episodes
		s->set(m_sTheta, -0.2 + getRandomValue()*0.4);
		s->set(m_sTheta_dot, -0.05 + getRandomValue()*0.1);
		s->set(m_sX, -0.5 + getRandomValue());
		s->set(m_sX_dot, -0.1 + getRandomValue()*0.2);
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
	double force = a->get(m_aPitch);
	double theta = s->get(m_sTheta);
	double theta_dot = s->get(m_sTheta_dot);
	double x = s->get(m_sX);
	double x_dot = s->get(m_sX_dot);
	double costheta = cos(theta);
	double sintheta = sin(theta);

	double temp = (force + POLEMASS_LENGTH * theta_dot * theta_dot * sintheta)
		/ TOTAL_MASS;

	double thetaacc = (GRAVITY * sintheta - costheta* temp)
		/ (LENGTH * (FOURTHIRDS - MASSPOLE * costheta * costheta
		/ TOTAL_MASS));

	double xacc = temp - POLEMASS_LENGTH * thetaacc* costheta / TOTAL_MASS;

	/*** Update the four state variables, using Euler's method. ***/
	s->set(m_sX, x + x_dot*dt);
	s->set(m_sX_dot, x_dot + xacc*dt);
	s->set(m_sTheta, theta + theta_dot*dt);
	s->set(m_sTheta_dot, theta_dot + thetaacc*dt);
}

#define twelve_degrees 0.2094384
double CBalancingPoleReward::getReward(const CState* s, const CAction* a, const CState* s_p)
{
	bool bEval = CSimionApp::get()->pExperiment->isEvaluationEpisode();
	int step = CSimionApp::get()->pExperiment->getStep();
	double theta = s_p->get("theta");
	double x = s_p->get("x");

	if (x < -2.4 || x > 2.4 || /*theta<-1.3 || theta>1.3)*/
		//visually, it feels better to let it fall nearly completely
		theta < -twelve_degrees ||
		theta > twelve_degrees)
	{
		CSimionApp::get()->pExperiment->setTerminalState();
		return -1.0;
	}

	return 0.0;
}

double CBalancingPoleReward::getMin()
{
	return -1.0;
}

double CBalancingPoleReward::getMax()
{
	return 0.0;
}