#include "stdafx.h"
#include "single-dimension-grid.h"
#include "globals.h"
#include "features.h"
#include "parameters.h"
#include "world.h"
#include "named-var-set.h"


#define ACTIVATION_THRESHOLD 0.0001

CSingleDimensionGrid::CSingleDimensionGrid()
{
	m_pCenters = 0;
	m_numCenters = 0;
	m_variableIndex = -1;
	m_min = 0.0;
	m_max = 0.0;
	m_distributionType = 0;
}

CSingleDimensionGrid::~CSingleDimensionGrid()
{
	delete[] m_pCenters;
}

void CSingleDimensionGrid::initCenterPoints()
{
	m_pCenters = new double[m_numCenters];



	if (!strcmp(m_distributionType, "linear"))
	{
		for (int i = 0; i<m_numCenters; i++)
			m_pCenters[i] = m_min + (((double)i) / (m_numCenters - 1))*(m_max - m_min);
	}
	else
	{
		double normalisedPos;
		double ncenters = (double)m_numCenters;
		for (int i = 0; i<m_numCenters; i++)
		{
			normalisedPos = ((double)i - ncenters*.5) / (ncenters*.5);

			if (!strcmp(m_distributionType, "cubic"))
				normalisedPos = pow(normalisedPos, 3.0);
			else if (!strcmp(m_distributionType, "quadratic"))
				normalisedPos = pow(normalisedPos, 2.0);
			else assert(0);

			m_pCenters[i] = m_min + (normalisedPos + 1.0)*0.5*(m_max - m_min);
		}
	}
}

CLASS_CONSTRUCTOR(CStateVariableGrid)(CParameters* pParameters)
{
	STATE_VARIABLE_REF(m_variableIndex, pParameters, "Variable");
	CONST_INTEGER_VALUE(m_numCenters, pParameters, "Num-Features", 3);

	CONST_DOUBLE_VALUE(m_min, pParameters, "Min", 0.0);// RLSimion::g_pWorld->getStateDescriptor()->getMin(m_variableIndex));
	CONST_DOUBLE_VALUE(m_max, pParameters, "Max", 1.0);// RLSimion::g_pWorld->getStateDescriptor()->getMax(m_variableIndex));

	CONST_STRING_VALUE(m_distributionType, pParameters, "Distribution", "linear");
	initCenterPoints();
	END_CLASS();
}
void CStateVariableGrid::setFeatureStateAction(unsigned int feature, CState* s, CState* a)
{
	s->setValue(m_variableIndex, m_pCenters[feature]);
}

double CStateVariableGrid::getVariableValue(const CState* s, const CAction* a)
{
	return s->getValue(m_variableIndex);
}


CLASS_CONSTRUCTOR(CActionVariableGrid)(CParameters* pParameters)
{
	ACTION_VARIABLE_REF(m_variableIndex, pParameters, "Variable");
	CONST_INTEGER_VALUE(m_numCenters, pParameters, "Num-Features", 3);

	CONST_DOUBLE_VALUE(m_min, pParameters, "Min", 0.0);// RLSimion::g_pWorld->getActionDescriptor()->getMin(m_variableIndex));
	CONST_DOUBLE_VALUE(m_max, pParameters, "Max", 1.0);// RLSimion::g_pWorld->getActionDescriptor()->getMax(m_variableIndex));

	CONST_STRING_VALUE(m_distributionType, pParameters, "Distribution", "linear");
	initCenterPoints();
	END_CLASS();
}

void CActionVariableGrid::setFeatureStateAction(unsigned int feature, CState* s, CState* a)
{
	a->setValue(m_variableIndex, m_pCenters[feature]);
}

double CActionVariableGrid::getVariableValue(const CState* s, const CAction* a)
{
	return a->getValue(m_variableIndex);
}



double CSingleDimensionGrid::getFeatureFactor(int feature, double value)
{
	double range, dist;
	assert(feature >= 0 && feature <= m_numCenters - 1);
	if (value>m_pCenters[feature])
	{
		dist = value - m_pCenters[feature];
		if (feature != m_numCenters - 1)
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

void CSingleDimensionGrid::getFeatures(const CState* s, const CAction* a, CFeatureList* outDimFeatures)
{
	double u;
	int i;

	outDimFeatures->clear();

	unsigned int numCenters = m_numCenters;

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
