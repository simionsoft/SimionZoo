#include "gridworld.h"
#include "../named-var-set.h"
#include "../config.h"
#include "../reward.h"
#include "../app.h"
#include "../experiment.h"

ContinuousGridWorld::ContinuousGridWorld(ConfigNode* pConfigNode)
{
	METADATA("World", "Continuous-GridWorld");

	m_targetX = INT_PARAM(pConfigNode, "Target-X", "X coordinate of the target position.", 0);
	m_targetY = INT_PARAM(pConfigNode, "Target-Y", "Y coordinate of the target position.", 8);

	m_sXPosition = addStateVariable("x-position", "m", -10, 10);
	m_sYPosition = addStateVariable("y-position", "m", -10, 10);

	m_aXStep = addActionVariable("x-step", "m", -1.0, 1.0);
	m_aYStep = addActionVariable("y-step", "m", -1.0, 1.0);

	//the reward function
	m_pRewardFunction->addRewardComponent(new ContinuousGridWorldReward(m_targetX.get(), m_targetY.get()));
	m_pRewardFunction->initialize();
}

ContinuousGridWorld::~ContinuousGridWorld()
{

}

void ContinuousGridWorld::reset(State *s)
{

	//fixed setting
	s->set(m_sXPosition, 0);
	s->set(m_sYPosition, -10.0);
}


void ContinuousGridWorld::executeAction(State *s, const Action *a, double dt)
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

ContinuousGridWorldReward::ContinuousGridWorldReward(int targetX, int targetY)
{
	m_xTarget = (double)targetX;
	m_yTarget = (double)targetY;
}


double ContinuousGridWorldReward::getReward(const State* s, const Action* a, const State* s_p)
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
		SimionApp::get()->pExperiment->setTerminalState();
		return 10.0;
	}

	return 0;
}

double ContinuousGridWorldReward::getMin() { return -1.0; }

double ContinuousGridWorldReward::getMax() { return 10.0; }
