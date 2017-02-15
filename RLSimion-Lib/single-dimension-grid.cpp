#include "stdafx.h"
#include "single-dimension-grid.h"
#include "features.h"
#include "config.h"
#include "world.h"
#include "named-var-set.h"
#include "app.h"

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

void CSingleDimensionGrid::getFeatures(const CState* s, const CAction* a, CFeatureList* outDimFeatures)
{
	double u;
	int i;

	outDimFeatures->clear();

	unsigned int numCenters = m_numCenters.get();

	assert(numCenters >= 2);
	double value = getVariableValue(s, a);

	if (value <= m_pCenters[1])
	{
		outDimFeatures->add(0, getFeatureFactor(0, value));
		outDimFeatures->add(1, getFeatureFactor(1, value));
		outDimFeatures->add(2, getFeatureFactor(2, value));
	}
	else if (value >= m_pCenters[numCenters - 2])
	{
		outDimFeatures->add(numCenters - 3, getFeatureFactor(numCenters - 3, value));
		outDimFeatures->add(numCenters - 2, getFeatureFactor(numCenters - 2, value));
		outDimFeatures->add(numCenters - 1, getFeatureFactor(numCenters - 1, value));
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

double CSingleDimensionGrid::getFeatureFactor(int feature, double value)
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

CStateVariableGrid::CStateVariableGrid(CConfigNode* pConfigNode)
{
	m_hVariable = STATE_VARIABLE(pConfigNode,"Variable", "The state variable");
	m_numCenters = INT_PARAM(pConfigNode, "Num-Features","The number of points that form the grid",3);

	CDescriptor &descriptor = CWorld::getDynamicModel()->getStateDescriptor();
	CNamedVarProperties properties = descriptor[m_hVariable.get()];
	m_min= properties.getMin();
	m_max= properties.getMax();
	m_distributionType = ENUM_PARAM<Distribution>(pConfigNode,"Distribution"
		, "The manner in which the points are distributed on the state variable's grid"
		,Distribution::linear);

	initCenterPoints();
}
void CStateVariableGrid::setFeatureStateAction(unsigned int feature, CState* s, CState* a)
{
	s->setValue(m_hVariable.get(), m_pCenters[feature]);
}

double CStateVariableGrid::getVariableValue(const CState* s, const CAction* a)
{
	return s->get(m_hVariable.get());
}

CActionVariableGrid::CActionVariableGrid(CConfigNode* pConfigNode)
{
	m_hVariable = ACTION_VARIABLE(pConfigNode,"Variable", "The action variable");
	m_numCenters = INT_PARAM(pConfigNode, "Num-Features","The number of points that form the grid",3);

	m_min = CSimionApp::get()->pWorld->getDynamicModel()->getActionDescriptor()[m_hVariable.get()].getMin();
	m_max = CSimionApp::get()->pWorld->getDynamicModel()->getActionDescriptor()[m_hVariable.get()].getMax();

	m_distributionType = ENUM_PARAM<Distribution>(pConfigNode, "Distribution"
		, "The manner in which the points are distributed on the action variable's grid",Distribution::linear);

	initCenterPoints();
}

void CActionVariableGrid::setFeatureStateAction(unsigned int feature, CState* s, CState* a)
{
	a->setValue(m_hVariable.get(), m_pCenters[feature]);
}

double CActionVariableGrid::getVariableValue(const CState* s, const CAction* a)
{
	return a->get(m_hVariable.get());
}