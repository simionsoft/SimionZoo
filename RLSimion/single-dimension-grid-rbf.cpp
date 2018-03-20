#include "single-dimension-grid-rbf.h"
#include "features.h"
#include "config.h"
#include "worlds/world.h"
#include "named-var-set.h"
#include "app.h"
#include <assert.h>

SingleDimensionGridRBF::SingleDimensionGridRBF()
{
	m_pCenters = 0;
	m_min = 0.0;
	m_max = 0.0;
}

SingleDimensionGridRBF::~SingleDimensionGridRBF()
{
	delete[] m_pCenters;
}

void SingleDimensionGridRBF::initCenterPoints()
{
	m_pCenters = new double[m_numCenters.get()];

	if (m_distributionType.get()== Distribution::linear)
	{
		for (int i = 0; i<m_numCenters.get(); i++)
			m_pCenters[i] = m_min + (((double)i) / (m_numCenters.get() - 1))*(m_max - m_min);
	}
	else
	{
		double normalisedPos;
		double ncenters = (double)m_numCenters.get();
		for (int i = 0; i<m_numCenters.get(); i++)
		{
			normalisedPos = ((double)i - ncenters*.5) / (ncenters*.5);

			if (m_distributionType.get()==Distribution::cubic)
				normalisedPos = pow(normalisedPos, 3.0);
			else if (m_distributionType.get()==Distribution::quadratic)
				normalisedPos = pow(normalisedPos, 2.0);
			else assert(0);

			m_pCenters[i] = m_min + (normalisedPos + 1.0)*0.5*(m_max - m_min);
		}
	}
}

void SingleDimensionGridRBF::getFeatures(const State* s, const Action* a, FeatureList* outDimFeatures)
{
	double u;
	int i;

	outDimFeatures->clear();

	unsigned int numCenters = m_numCenters.get();

	assert(numCenters >= 2);
	double value = getVarValue(s, a);
	NamedVarProperties& properties = getVarProperties(s, a);

	if (value <= m_pCenters[1])
	{
		if (!properties.bIsCircular())
		{
			outDimFeatures->add(0, getFeatureFactor(0, value));
			outDimFeatures->add(1, getFeatureFactor(1, value));
			outDimFeatures->add(2, getFeatureFactor(2, value));
		}
		else
		{
			outDimFeatures->add(0, getFeatureFactor(0, value));
			outDimFeatures->add(1, getFeatureFactor(1, value));
			outDimFeatures->add(numCenters-1, getFeatureFactor(numCenters-1, value + properties.getRangeWidth()));
		}
	}
	else if (value >= m_pCenters[numCenters - 2])
	{
		if (!properties.bIsCircular())
		{
			outDimFeatures->add(numCenters - 3, getFeatureFactor(numCenters - 3, value));
			outDimFeatures->add(numCenters - 2, getFeatureFactor(numCenters - 2, value));
			outDimFeatures->add(numCenters - 1, getFeatureFactor(numCenters - 1, value));
		}
		else
		{
			outDimFeatures->add(numCenters - 2, getFeatureFactor(numCenters - 2, value));
			outDimFeatures->add(numCenters - 1, getFeatureFactor(numCenters - 1, value));
			outDimFeatures->add(0, getFeatureFactor(0, value - properties.getRangeWidth()));
		}
	}
	else
	{
		i = 1;

		while (value>m_pCenters[i + 1]) i++;

		u = (value - m_pCenters[i]) / (m_pCenters[i + 1] - m_pCenters[i]);

		if (u<0.5)
		{
			outDimFeatures->add(i, getFeatureFactor(i, value));
			outDimFeatures->add(i + 1, getFeatureFactor(i + 1, value));
		}
		else
		{
			outDimFeatures->add(i + 1, getFeatureFactor(i + 1, value));
			outDimFeatures->add(i, getFeatureFactor(i, value));
		}

		if (value - m_pCenters[i - 1] < m_pCenters[i + 2] - value)
			outDimFeatures->add(i - 1, getFeatureFactor(i - 1, value));
		else
			outDimFeatures->add(i + 2, getFeatureFactor(i + 2, value));
	}
	outDimFeatures->applyThreshold(ACTIVATION_THRESHOLD);
	outDimFeatures->normalize();
}

double SingleDimensionGridRBF::getFeatureFactor(int feature, double value)
{
	double range, dist;
	assert(feature >= 0 && feature <= m_numCenters.get() - 1);
	if (value>m_pCenters[feature])
	{
		dist = value - m_pCenters[feature];
		if (feature != m_numCenters.get() - 1)
			range = m_pCenters[feature + 1] - m_pCenters[feature];
		else
			range = m_pCenters[feature] - m_pCenters[feature - 1];
	}
	else
	{
		dist = m_pCenters[feature] - value;
		if (feature != 0)
			range = m_pCenters[feature] - m_pCenters[feature - 1];
		else
			range = m_pCenters[1] - m_pCenters[0];
	}

	//f_gauss(x)= a*exp(-(x-b)^2 / 2c^2 )
	//instead of 2c^2, we use the distance to the next feature
	double f = 2 * dist / range;
	double factor = exp(-(f*f));
	return factor;

	//the original feature factor calculation code in RLToolbox:
	//double distance = fabs(difference);
	//return my_exp(- pow(distance / diffNextPart * 2, 2)) ;
}

StateVariableGridRBF::StateVariableGridRBF(ConfigNode* pConfigNode)
{
	m_hVariable = STATE_VARIABLE(pConfigNode,"Variable", "The state variable");
	m_numCenters = INT_PARAM(pConfigNode, "Num-Features","The number of points that form the grid",3);
	m_distributionType = ENUM_PARAM<Distribution>(pConfigNode, "Distribution"
		, "The manner in which the points are distributed on the state variable's grid"
		, Distribution::linear);
	
	initVarRange();
	initCenterPoints();
}

void StateVariableGridRBF::initVarRange()
{
	Descriptor &descriptor = World::getDynamicModel()->getStateDescriptor();
	NamedVarProperties properties = descriptor[m_hVariable.get()];
	m_min = properties.getMin();
	m_max = properties.getMax();
}


StateVariableGridRBF::StateVariableGridRBF(int m_hVar, int numCenters, Distribution distr)
{
	m_hVariable.set(m_hVar);
	m_numCenters.set(numCenters);
	m_distributionType.set(distr);

	initVarRange();
	initCenterPoints();
}
void StateVariableGridRBF::setFeatureStateAction(unsigned int feature, State* s, State* a)
{
	s->set(m_hVariable.get(), m_pCenters[feature]);
}

double StateVariableGridRBF::getVarValue(const State* s, const Action* a)
{
	return s->get(m_hVariable.get());
}

NamedVarProperties& StateVariableGridRBF::getVarProperties(const State* s, const Action* a)
{
	return s->getProperties(m_hVariable.get());
}

ActionVariableGridRBF::ActionVariableGridRBF(ConfigNode* pConfigNode)
{
	m_hVariable = ACTION_VARIABLE(pConfigNode,"Variable", "The action variable");
	m_numCenters = INT_PARAM(pConfigNode, "Num-Features","The number of points that form the grid",3);
	m_distributionType = ENUM_PARAM<Distribution>(pConfigNode, "Distribution"
		, "The manner in which the points are distributed on the action variable's grid",Distribution::linear);

	initVarRange();
	initCenterPoints();
}

void ActionVariableGridRBF::initVarRange()
{
	Descriptor &descriptor = World::getDynamicModel()->getActionDescriptor();
	NamedVarProperties properties = descriptor[m_hVariable.get()];
	m_min = properties.getMin();
	m_max = properties.getMax();
}

void ActionVariableGridRBF::setFeatureStateAction(unsigned int feature, State* s, State* a)
{
	a->set(m_hVariable.get(), m_pCenters[feature]);
}

double ActionVariableGridRBF::getVarValue(const State* s, const Action* a)
{
	return a->get(m_hVariable.get());
}

NamedVarProperties& ActionVariableGridRBF::getVarProperties(const State* s, const Action* a)
{
	return a->getProperties(m_hVariable.get());
}