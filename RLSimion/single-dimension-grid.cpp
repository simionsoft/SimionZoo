#include "single-dimension-grid.h"
#include "features.h"
#include "config.h"
#include "worlds/world.h"
#include "named-var-set.h"
#include "app.h"
#include <assert.h>

CSingleDimensionGrid::CSingleDimensionGrid()
{
	m_pCenters = 0;
	m_min = 0.0;
	m_max = 0.0;
}

CSingleDimensionGrid::~CSingleDimensionGrid()
{
	delete[] m_pCenters;
}

void CSingleDimensionGrid::initCenterPoints()
{
	m_pCenters = new double[m_numCenters.get()];

	if (m_distributionType.get() == Distribution::linear)
	{
		for (int i = 0; i < m_numCenters.get(); i++)
			m_pCenters[i] = m_min + (((double)i) / (m_numCenters.get() - 1))*(m_max - m_min);
	}
	else
	{
		double normalisedPos;
		double ncenters = (double)m_numCenters.get();
		for (int i = 0; i < m_numCenters.get(); i++)
		{
			normalisedPos = ((double)i - ncenters*.5) / (ncenters*.5);

			if (m_distributionType.get() == Distribution::cubic)
				normalisedPos = pow(normalisedPos, 3.0);
			else if (m_distributionType.get() == Distribution::quadratic)
				normalisedPos = pow(normalisedPos, 2.0);
			else assert(0);

			m_pCenters[i] = m_min + (normalisedPos + 1.0)*0.5*(m_max - m_min);
		}
	}
}

void CSingleDimensionGrid::getFeatures(const CState* s, const CAction* a, CFeatureList* outDimFeatures)
{
	double value = getVarValue(s, a) + m_offset;
	outDimFeatures->clear();
	outDimFeatures->add(getClosestCenter(value), 1.0);
}

int CSingleDimensionGrid::getClosestCenter(double value)
{
	unsigned int numCenters = m_numCenters.get();
	assert(numCenters >= 2);
	unsigned int nearestIndex = 0;

	for (unsigned int i = 0; i < numCenters; i++)
	{
		//there is no special treatment for circular variables 
		if (abs(value - m_pCenters[i]) < abs(value - m_pCenters[nearestIndex]))
		{
			nearestIndex = i;
		}
	}

	return nearestIndex;
}

CSingleDimensionStateVariableGrid::CSingleDimensionStateVariableGrid(CConfigNode* pConfigNode)
{
	m_hVariable = STATE_VARIABLE(pConfigNode, "Variable", "The state variable");
	m_numCenters = INT_PARAM(pConfigNode, "Num-Features", "The number of points that form the grid", 3);
	m_distributionType = ENUM_PARAM<Distribution>(pConfigNode, "Distribution"
		, "The manner in which the points are distributed on the state variable's grid"
		, Distribution::linear);

	initVarRange();
	initCenterPoints();
}

void CSingleDimensionStateVariableGrid::initVarRange()
{
	CDescriptor &descriptor = CWorld::getDynamicModel()->getStateDescriptor();
	CNamedVarProperties properties = descriptor[m_hVariable.get()];
	m_min = properties.getMin();
	m_max = properties.getMax();
}


CSingleDimensionStateVariableGrid::CSingleDimensionStateVariableGrid(int m_hVar, int numCenters, Distribution distr)
{
	m_hVariable.set(m_hVar);
	m_numCenters.set(numCenters);
	m_distributionType.set(distr);

	initVarRange();
	initCenterPoints();
}
void CSingleDimensionStateVariableGrid::setFeatureStateAction(unsigned int feature, CState* s, CState* a)
{
	s->set(m_hVariable.get(), m_pCenters[feature]);
}

double CSingleDimensionStateVariableGrid::getVarValue(const CState* s, const CAction* a)
{
	return s->get(m_hVariable.get());
}

CNamedVarProperties& CSingleDimensionStateVariableGrid::getVarProperties(const CState* s, const CAction* a)
{
	return s->getProperties(m_hVariable.get());
}

CSingleDimensionActionVariableGrid::CSingleDimensionActionVariableGrid(CConfigNode* pConfigNode)
{
	m_hVariable = ACTION_VARIABLE(pConfigNode, "Variable", "The action variable");
	m_numCenters = INT_PARAM(pConfigNode, "Num-Features", "The number of points that form the grid", 3);
	m_distributionType = ENUM_PARAM<Distribution>(pConfigNode, "Distribution"
		, "The manner in which the points are distributed on the action variable's grid", Distribution::linear);

	initVarRange();
	initCenterPoints();
}

void CSingleDimensionActionVariableGrid::initVarRange()
{
	CDescriptor &descriptor = CWorld::getDynamicModel()->getActionDescriptor();
	CNamedVarProperties properties = descriptor[m_hVariable.get()];
	m_min = properties.getMin();
	m_max = properties.getMax();
}

void CSingleDimensionActionVariableGrid::setFeatureStateAction(unsigned int feature, CState* s, CState* a)
{
	a->set(m_hVariable.get(), m_pCenters[feature]);
}

double CSingleDimensionActionVariableGrid::getVarValue(const CState* s, const CAction* a)
{
	return a->get(m_hVariable.get());
}

CNamedVarProperties& CSingleDimensionActionVariableGrid::getVarProperties(const CState* s, const CAction* a)
{
	return a->getProperties(m_hVariable.get());
}

void CSingleDimensionStateVariableGrid::setOffset(double offset)
{
	m_offset = offset;
}

void CSingleDimensionActionVariableGrid::setOffset(double offset)
{
	m_offset = offset;
}