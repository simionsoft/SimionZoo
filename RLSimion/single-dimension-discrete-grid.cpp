#include "single-dimension-discrete-grid.h"
#include "features.h"
#include "config.h"
#include "worlds/world.h"
#include "named-var-set.h"
#include "app.h"
#include "logger.h"
#include <assert.h>

SingleDimensionDiscreteGrid::SingleDimensionDiscreteGrid()
{
	m_pCenters = 0;
	m_min = 0.0;
	m_max = 0.0;
}

SingleDimensionDiscreteGrid::~SingleDimensionDiscreteGrid()
{
	delete[] m_pCenters;
}

void SingleDimensionDiscreteGrid::initCenterPoints()
{
	m_pCenters = new double[m_numCenters.get()];

	for (size_t i = 0; i < m_numCenters.get(); i++)
		m_pCenters[i] = m_min + i * m_stepSize;
}

void SingleDimensionDiscreteGrid::getFeatures(const State* s, const Action* a, FeatureList* outDimFeatures)
{
	double value = getVarValue(s, a) + m_offset;
	outDimFeatures->clear();
	outDimFeatures->add(getClosestCenter(value), 1.0);
}

int SingleDimensionDiscreteGrid::getClosestCenter(double value)
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

SingleDimensionDiscreteStateVariableGrid::SingleDimensionDiscreteStateVariableGrid(ConfigNode* pConfigNode)
{
	m_hVariable = STATE_VARIABLE(pConfigNode, "Variable", "The state variable");
	m_numCenters = INT_PARAM(pConfigNode, "Steps", "The number of sampling points in which the space will be discretized", 2);
	
	if (m_numCenters.get() < 2)
		Logger::logMessage(MessageType::Error, "Number of sampling points has to be greater than 1.");

	initVarRange();

	m_stepSize = (m_max - m_min) / (m_numCenters.get() - 1);

	initCenterPoints();
}

void SingleDimensionDiscreteStateVariableGrid::initVarRange()
{
	Descriptor &descriptor = World::getDynamicModel()->getStateDescriptor();
	NamedVarProperties properties = descriptor[m_hVariable.get()];
	m_min = properties.getMin();
	m_max = properties.getMax();
}


SingleDimensionDiscreteStateVariableGrid::SingleDimensionDiscreteStateVariableGrid(int m_hVar, int steps)
{
	initVarRange();

	m_hVariable.set(m_hVar);
	m_numCenters.set(steps);

	m_stepSize = (m_max - m_min) / (m_numCenters.get() - 1);

	initCenterPoints();
}

void SingleDimensionDiscreteStateVariableGrid::setFeatureStateAction(unsigned int feature, State* s, State* a)
{
	s->set(m_hVariable.get(), m_pCenters[feature]);
}

double SingleDimensionDiscreteStateVariableGrid::getVarValue(const State* s, const Action* a)
{
	return s->get(m_hVariable.get());
}

NamedVarProperties& SingleDimensionDiscreteStateVariableGrid::getVarProperties(const State* s, const Action* a)
{
	return s->getProperties(m_hVariable.get());
}

SingleDimensionDiscreteActionVariableGrid::SingleDimensionDiscreteActionVariableGrid(ConfigNode* pConfigNode)
{
	m_hVariable = ACTION_VARIABLE(pConfigNode, "Variable", "The action variable");
	m_numCenters = INT_PARAM(pConfigNode, "Steps", "The number of sampling points in which the space will be discretized", 2);
	
	if (m_numCenters.get() < 2)
		Logger::logMessage(MessageType::Error, "Number of sampling points has to be greater than 1.");

	initVarRange();
	m_stepSize = (m_max - m_min) / (m_numCenters.get() - 1);

	initCenterPoints();
}

void SingleDimensionDiscreteActionVariableGrid::initVarRange()
{
	Descriptor &descriptor = World::getDynamicModel()->getActionDescriptor();
	NamedVarProperties properties = descriptor[m_hVariable.get()];
	m_min = properties.getMin();
	m_max = properties.getMax();
}

SingleDimensionDiscreteActionVariableGrid::SingleDimensionDiscreteActionVariableGrid(int m_hVar, int steps)
{
	initVarRange();

	m_hVariable.set(m_hVar);
	m_numCenters.set(steps);

	m_stepSize = (m_max - m_min) / (m_numCenters.get() - 1);

	initCenterPoints();
}

void SingleDimensionDiscreteActionVariableGrid::setFeatureStateAction(unsigned int feature, State* s, State* a)
{
	a->set(m_hVariable.get(), m_pCenters[feature]);
}

double SingleDimensionDiscreteActionVariableGrid::getVarValue(const State* s, const Action* a)
{
	return a->get(m_hVariable.get());
}

NamedVarProperties& SingleDimensionDiscreteActionVariableGrid::getVarProperties(const State* s, const Action* a)
{
	return a->getProperties(m_hVariable.get());
}

void SingleDimensionDiscreteStateVariableGrid::setOffset(double offset)
{
	m_offset = offset;
}

void SingleDimensionDiscreteActionVariableGrid::setOffset(double offset)
{
	m_offset = offset;
}