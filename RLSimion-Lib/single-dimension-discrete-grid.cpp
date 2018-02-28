#include "single-dimension-discrete-grid.h"
#include "features.h"
#include "config.h"
#include "worlds/world.h"
#include "named-var-set.h"
#include "app.h"
#include <assert.h>

CSingleDimensionDiscreteGrid::CSingleDimensionDiscreteGrid()
{
	m_pCenters = 0;
	m_min = 0.0;
	m_max = 0.0;
}

CSingleDimensionDiscreteGrid::~CSingleDimensionDiscreteGrid()
{
	delete[] m_pCenters;
}

void CSingleDimensionDiscreteGrid::initCenterPoints()
{
	m_pCenters = new double[m_numCenters.get()];

	for (size_t i = 0; i < m_numCenters.get(); i++)
		m_pCenters[i] = m_min + i * m_stepSize;
}

void CSingleDimensionDiscreteGrid::getFeatures(const CState* s, const CAction* a, CFeatureList* outDimFeatures)
{
	double value = getVarValue(s, a) + m_offset;
	outDimFeatures->clear();
	outDimFeatures->add(getClosestCenter(value), 1.0);
}

int CSingleDimensionDiscreteGrid::getClosestCenter(double value)
{
	assert(m_numCenters.get() >= 2);
	unsigned int nearestIndex = 0;

	for (int i = 0; i < m_numCenters.get(); i++)
	{
		//there is no special treatment for circular variables 
		if (abs(value - m_pCenters[i]) < abs(value - m_pCenters[nearestIndex]))
		{
			nearestIndex = i;
		}
	}

	return nearestIndex;
}

CSingleDimensionDiscreteStateVariableGrid::CSingleDimensionDiscreteStateVariableGrid(CConfigNode* pConfigNode)
{
	m_hVariable = STATE_VARIABLE(pConfigNode, "Variable", "The state variable");
	m_numCenters = INT_PARAM(pConfigNode, "Steps", "The number of steps in which the space will be discretized", 1);

	initVarRange();
	m_stepSize = (m_max - m_min + 1.0) / m_numCenters.get();

	initCenterPoints();
}

void CSingleDimensionDiscreteStateVariableGrid::initVarRange()
{
	CDescriptor &descriptor = CWorld::getDynamicModel()->getStateDescriptor();
	CNamedVarProperties properties = descriptor[m_hVariable.get()];
	m_min = properties.getMin();
	m_max = properties.getMax();
}


CSingleDimensionDiscreteStateVariableGrid::CSingleDimensionDiscreteStateVariableGrid(int m_hVar, int steps)
{
	initVarRange();

	m_hVariable.set(m_hVar);
	m_numCenters.set(steps);

	m_stepSize = (m_max - m_min + 1.0) / m_numCenters.get();

	initCenterPoints();
}

void CSingleDimensionDiscreteStateVariableGrid::setFeatureStateAction(unsigned int feature, CState* s, CState* a)
{
	s->set(m_hVariable.get(), m_pCenters[feature]);
}

double CSingleDimensionDiscreteStateVariableGrid::getVarValue(const CState* s, const CAction* a)
{
	return s->get(m_hVariable.get());
}

CNamedVarProperties& CSingleDimensionDiscreteStateVariableGrid::getVarProperties(const CState* s, const CAction* a)
{
	return s->getProperties(m_hVariable.get());
}

CSingleDimensionDiscreteActionVariableGrid::CSingleDimensionDiscreteActionVariableGrid(CConfigNode* pConfigNode)
{
	m_hVariable = ACTION_VARIABLE(pConfigNode, "Variable", "The action variable");
	m_numCenters = INT_PARAM(pConfigNode, "Steps", "The number of steps in which the space will be discretized", 1);

	initVarRange();
	m_stepSize = (m_max - m_min + 1.0) / m_numCenters.get();

	initCenterPoints();
}

void CSingleDimensionDiscreteActionVariableGrid::initVarRange()
{
	CDescriptor &descriptor = CWorld::getDynamicModel()->getActionDescriptor();
	CNamedVarProperties properties = descriptor[m_hVariable.get()];
	m_min = properties.getMin();
	m_max = properties.getMax();
}

CSingleDimensionDiscreteActionVariableGrid::CSingleDimensionDiscreteActionVariableGrid(int m_hVar, int steps)
{
	initVarRange();

	m_hVariable.set(m_hVar);
	m_numCenters.set(steps);

	m_stepSize = (m_max - m_min + 1.0) / m_numCenters.get();

	initCenterPoints();
}

void CSingleDimensionDiscreteActionVariableGrid::setFeatureStateAction(unsigned int feature, CState* s, CState* a)
{
	a->set(m_hVariable.get(), m_pCenters[feature]);
}

double CSingleDimensionDiscreteActionVariableGrid::getVarValue(const CState* s, const CAction* a)
{
	return a->get(m_hVariable.get());
}

CNamedVarProperties& CSingleDimensionDiscreteActionVariableGrid::getVarProperties(const CState* s, const CAction* a)
{
	return a->getProperties(m_hVariable.get());
}

void CSingleDimensionDiscreteStateVariableGrid::setOffset(double offset)
{
	m_offset = offset;
}

void CSingleDimensionDiscreteActionVariableGrid::setOffset(double offset)
{
	m_offset = offset;
}