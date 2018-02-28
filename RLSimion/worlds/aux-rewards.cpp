#include "aux-rewards.h"
#include "../named-var-set.h"
#include "world.h"
#include <algorithm>

double static getDistanceBetweenPoints(double x1, double y1, double x2, double y2)
{
	double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	return distance;
}

CDistanceReward2D::CDistanceReward2D(CDescriptor& stateDescr, const char* var1xName, const char* var1yName, const char* var2xName, const char* var2yName)
{
	m_var1xId = stateDescr.getVarIndex(var1xName);
	m_var1yId = stateDescr.getVarIndex(var1yName);
	m_var2xId = stateDescr.getVarIndex(var2xName);
	m_var2yId = stateDescr.getVarIndex(var2yName);

	//here we assume both variables have the same value range
	m_maxDist = sqrt(stateDescr[m_var1xId].getRangeWidth()*stateDescr[m_var1xId].getRangeWidth()
		+ stateDescr[m_var1yId].getRangeWidth()*stateDescr[m_var1yId].getRangeWidth());
}

CDistanceReward2D::CDistanceReward2D(CDescriptor& stateDescr, int var1xId, int var1yId, int var2xId, int var2yId)
{
	m_var1xId = var1xId;
	m_var1yId = var1yId;
	m_var2xId = var2xId;
	m_var2yId = var2yId;

	//here we assume both variables have the same value range
	m_maxDist = sqrt(stateDescr[m_var1xId].getRangeWidth()*stateDescr[m_var1xId].getRangeWidth()
		+ stateDescr[m_var1yId].getRangeWidth()*stateDescr[m_var1yId].getRangeWidth());

}

double CDistanceReward2D::getReward(const CState* s, const CAction* a, const CState* s_p)
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

double CDistanceReward2D::getMin()
{
	return 0.0;
}

double CDistanceReward2D::getMax()
{
	return 1.0;
}
