#include "../stdafx.h"
#include "gridworld.h"
#include "../named-var-set.h"
#include "../config.h"
#include "../reward.h"
#include "../app.h"

CContinuousGridWorld::CContinuousGridWorld(CConfigNode* pConfigNode)
{
	METADATA("World", "Continuous-GridWorld");

	m_targetX = INT_PARAM(pConfigNode, "Target-X", "X coordinate of the target position.", 0);
	m_targetY = INT_PARAM(pConfigNode, "Target-Y", "Y coordinate of the target position.", 10);

	m_sXPosition = addStateVariable("x-position", "m", -10, 10);
	m_sYPosition = addStateVariable("z-position", "m", -10, 10);

	m_aXStep = addActionVariable("x-step", "m", -1.0, 1.0);
	m_aYStep = addActionVariable("y-step", "m", -1.0, 1.0);

	//the reward function
	m_pRewardFunction->addRewardComponent(new CContinuousGridWorldReward(m_targetX.get(), m_targetY.get()));
	m_pRewardFunction->initialize();
}

CContinuousGridWorld::~CContinuousGridWorld()
{

}

void CContinuousGridWorld::reset(CState *s)
{

	//fixed setting
	s->set(m_sXPosition, 0);
	s->set(m_sYPosition, -10.0);
}


void CContinuousGridWorld::executeAction(CState *s, const CAction *a, double dt)
{
	//this simulation model ignores dt!!

	double xPosition = s->get(m_sXPosition);
	double yPosition = s->get(m_sYPosition);
	double xStep = a->get(m_aXStep);
	double yStep = a->get(m_aXStep);

	if (xPosition + xStep >= s->getProperties(m_sXPosition).getMin() && xPosition + xStep <= s->getProperties(m_sXPosition).getMax())
	{
		xPosition += xStep;
	}
	if (yPosition + yStep >= s->getProperties(m_sYPosition).getMin() && yPosition + yStep <= s->getProperties(m_sYPosition).getMax())
	{
		yPosition += yStep;
	}

	s->set(m_sXPosition, xPosition);
	s->set(m_sYPosition, yPosition);
}

#define ALLOWED_ERROR 0.01

CContinuousGridWorldReward::CContinuousGridWorldReward(int targetX, int targetY)
{
	m_xTarget = (double)targetX;
	m_yTarget = (double)targetY;
}


double CContinuousGridWorldReward::getReward(const CState* s, const CAction* a, const CState* s_p)
{
	double xPosition = s_p->get("x-position");
	double yPosition = s_p->get("y-position");

	if (xPosition == s_p->getProperties("x-position").getMin() || xPosition == s_p->getProperties("x-position").getMax())
	{
		return -1.0;
	}
	if (yPosition == s_p->getProperties("y-position").getMin() || yPosition == s_p->getProperties("y-position").getMax())
	{
		return -1.0;
	}

	//reached the goal?
	if (fabs(m_xTarget - xPosition) < ALLOWED_ERROR && fabs(m_yTarget - yPosition) < ALLOWED_ERROR)
	{
		CSimionApp::get()->pExperiment->setTerminalState();
		return 10.0;
	}

	return 0;
}

double CContinuousGridWorldReward::getMin() { return -1.0; }

double CContinuousGridWorldReward::getMax() { return 10.0; }


//DISCRETE GRIDWORLD
CDiscreteGridWorld::CDiscreteGridWorld(CConfigNode* pConfigNode)
{
	METADATA("World", "Discrete-GridWorld");

	m_targetX = INT_PARAM(pConfigNode, "Target-X", "X coordinate of the target position.", 0);
	m_targetY = INT_PARAM(pConfigNode, "Target-Y", "Y coordinate of the target position.", 10);

	m_sXPosition = addStateVariable("x-position", "m", -10, 10);
	m_sYPosition = addStateVariable("z-position", "m", -10, 10);

	m_aStep = addActionVariable("step", "", 0, 4);

	//the reward function
	m_pRewardFunction->addRewardComponent(new CDiscreteGridWorldReward(m_targetX.get(), m_targetY.get()));
	m_pRewardFunction->initialize();
}

CDiscreteGridWorld::~CDiscreteGridWorld()
{

}

void CDiscreteGridWorld::reset(CState *s)
{

	//fixed setting
	s->set(m_sXPosition, 0);
	s->set(m_sYPosition, -10);
}


void CDiscreteGridWorld::executeAction(CState *s, const CAction *a, double dt)
{
	//this simulation model ignores dt!!
	int xPosition = (int)s->get(m_sXPosition);
	int yPosition = (int)s->get(m_sYPosition);
	int step = (int)round(a->get(m_aStep));

	int xStep = 0;
	int yStep = 0;

	switch (step)
	{
	case 0:
		xStep++;
		break;
	case 1:
		yStep++;
		break;
	case 2:
		xStep--;
		break;
	case 3:
		yStep--;
		break;
	}

	if (xPosition + xStep >= s->getProperties(m_sXPosition).getMin() && xPosition + xStep <= s->getProperties(m_sXPosition).getMax())
	{
		xPosition += xStep;
	}
	if (yPosition + yStep >= s->getProperties(m_sYPosition).getMin() && yPosition + yStep <= s->getProperties(m_sYPosition).getMax())
	{
		yPosition += yStep;
	}

	s->set(m_sXPosition, xPosition);
	s->set(m_sYPosition, yPosition);
}

CDiscreteGridWorldReward::CDiscreteGridWorldReward(int targetX, int targetY)
{
	m_xTarget = targetX;
	m_yTarget = targetY;
}


double CDiscreteGridWorldReward::getReward(const CState* s, const CAction* a, const CState* s_p)
{
	int xPosition = s_p->get("x-position");
	int yPosition = s_p->get("y-position");

	if (xPosition == s_p->getProperties("x-position").getMin() || xPosition == s_p->getProperties("x-position").getMax())
	{
		return -1.0;
	}
	if (yPosition == s_p->getProperties("y-position").getMin() || yPosition == s_p->getProperties("y-position").getMax())
	{
		return -1.0;
	}

	//reached the goal?
	if (m_xTarget == xPosition && m_yTarget == yPosition)
	{
	CSimionApp::get()->pExperiment->setTerminalState();
	return 10.0;
	}

	return 0;
}

double CDiscreteGridWorldReward::getMin() { return -1.0; }

double CDiscreteGridWorldReward::getMax() { return 10.0; }