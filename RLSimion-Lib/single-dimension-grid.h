#pragma once
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
class CFeatureList;
class CConfigNode;
#include "parameters.h"

#define ACTIVATION_THRESHOLD 0.0001

template<typename varType>
class CSingleDimensionGrid
{
protected:
	INT_PARAM m_numCenters;
	double *m_pCenters;

	varType m_variableIndex;
	double m_min;
	double m_max;
	ENUM_PARAM<Distribution> m_distributionType;

	CSingleDimensionGrid();

public:
	virtual ~CSingleDimensionGrid();

	void initCenterPoints();

	int getNumCenters(){ return m_numCenters.get(); }
	double* getCenters(){ return m_pCenters; }

	void getFeatures(const CState* s, const CAction* a, CFeatureList* outDimFeatures)
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

	double CSingleDimensionGrid<varType>::getFeatureFactor(int feature, double value)
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


	virtual double getVariableValue(const CState* s, const CAction* a) = 0;
	virtual void setFeatureStateAction(unsigned int feature, CState* s, CAction* a) = 0;
};

class CStateVariableGrid : public CSingleDimensionGrid<STATE_VARIABLE>
{
public:
	CStateVariableGrid(CConfigNode* pParameters);
	double getVariableValue(const CState* s, const CAction* a);
	void setFeatureStateAction(unsigned int feature, CState* s, CAction* a);
};

class CActionVariableGrid : public CSingleDimensionGrid<ACTION_VARIABLE>
{
public:
	CActionVariableGrid(CConfigNode* pParameters);
	double getVariableValue(const CState* s, const CAction* a);
	void setFeatureStateAction(unsigned int feature, CState* s, CAction* a);
};