#include "aux-rewards.h"
#include "../named-var-set.h"
#include "world.h"
#include <algorithm>
#include <math.h>

double static getDistanceBetweenPoints(double x1, double y1, double x2, double y2)
{
	double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	return distance;
}

DistanceReward2D::DistanceReward2D(Descriptor& stateDescr, const char* var1xName, const char* var1yName, const char* var2xName, const char* var2yName)
{
	m_var1xId = var1xName;
	m_var1yId = var1yName;
	m_var2xId = var2xName;
	m_var2yId = var2yName;

	//here we assume both variables have the same value range
	m_maxDist = sqrt(stateDescr.getProperties(m_var1xId)->getRangeWidth()
		* stateDescr.getProperties(m_var1xId)->getRangeWidth()
		+ stateDescr.getProperties(m_var1yId)->getRangeWidth()
		* stateDescr.getProperties(m_var1yId)->getRangeWidth());
}

double DistanceReward2D::getReward(const State* s, const Action* a, const State* s_p)
{
	double boxX = s_p->get(m_var1xId);
	double boxY = s_p->get(m_var1yId);
	double targetX = s_p->get(m_var2xId);
	double targetY = s_p->get(m_var2yId);

	double distance = getDistanceBetweenPoints(targetX, targetY, boxX, boxY);

	distance = std::max(distance, 0.0001);

	double reward = 1. - distance / m_maxDist;

	return reward;

}

double DistanceReward2D::getMin()
{
	return 0.0;
}

double DistanceReward2D::getMax()
{
	return 1.0;
}
