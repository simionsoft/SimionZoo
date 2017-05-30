#include "stdafx.h"
#include "world-aux-rewards.h"
#include "named-var-set.h"
#include "world.h"

double static getDistanceBetweenPoints(double x1, double y1, double x2, double y2)
{
	double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	return distance;
}

CBoxTargetReward::CBoxTargetReward(const char* var1xName, const char* var1yName, const char* var2xName, const char* var2yName)
{
	CDescriptor& descr= CWorld::getDynamicModel()->getStateDescriptor();
	m_var1xId = descr.getVarIndex(var1xName);
	m_var1yId = descr.getVarIndex(var1yName);
	m_var2xId = descr.getVarIndex(var2xName);
	m_var2yId = descr.getVarIndex(var2yName);
}

CBoxTargetReward::CBoxTargetReward(int var1xId, int var1yId, int var2xId, int var2yId)
{
	m_var1xId = var1xId;
	m_var1yId = var1yId;
	m_var2xId = var2xId;
	m_var2yId = var2yId;
}

double CBoxTargetReward::getReward(const CState* s, const CAction* a, const CState* s_p)
{
	double boxX = s_p->get(m_var1xId);
	double boxY = s_p->get(m_var1yId);
	double targetX = s_p->get(m_var2xId);
	double targetY = s_p->get(m_var2yId);

	double distance = getDistanceBetweenPoints(targetX, targetY, boxX, boxY);

	distance = std::max(distance, 0.0001);

	double distError = distance / 1.0;

	double reward = 1.0 - distError;

	return reward;

}

double CBoxTargetReward::getMin()
{
	return -10.0;
}

double CBoxTargetReward::getMax()
{
	return 1.0;
}
