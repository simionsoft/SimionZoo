#include "stdafx.h"
#include "single-dimension-grid.h"
#include "globals.h"
#include "features.h"
#include "config.h"
#include "world.h"
#include "named-var-set.h"
#include "app.h"



template<typename varType>
CSingleDimensionGrid<varType>::CSingleDimensionGrid()
{
	m_pCenters = 0;
	m_min = 0.0;
	m_max = 0.0;
}

template<typename varType>
CSingleDimensionGrid<varType>::~CSingleDimensionGrid()
{
	delete[] m_pCenters;
}

template<typename varType>
void CSingleDimensionGrid<varType>::initCenterPoints()
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

CStateVariableGrid::CStateVariableGrid(CConfigNode* pConfigNode)
{
	m_hVariable = STATE_VARIABLE(pConfigNode,"Variable", "The state variable");
	//STATE_VARIABLE_REF(m_hVariable, "Variable","The state variable");
	m_numCenters = INT_PARAM(pConfigNode, "Num-Features","The number of points that form the grid",3);
	//CONST_INTEGER_VALUE(m_numCenters, "Num-Features", 3,"The number of points that form the grid");

	m_min= CSimionApp::get()->pWorld->getDynamicModel()->getStateDescriptor()->getMin(m_hVariable.get());
	m_max= CSimionApp::get()->pWorld->getDynamicModel()->getStateDescriptor()->getMax(m_hVariable.get());
	m_distributionType = ENUM_PARAM<Distribution>(pConfigNode,"Distribution"
		, "The manner in which the points are distributed on the state variable's grid"
		,Distribution::linear);

	//ENUM_VALUE(m_distributionType, Distribution,"Distribution", "linear","The manner in which the points are distributed on the state variable's grid");
	initCenterPoints();
	//END_CLASS();
}
void CStateVariableGrid::setFeatureStateAction(unsigned int feature, CState* s, CState* a)
{
	s->setValue(m_hVariable.get(), m_pCenters[feature]);
}

double CStateVariableGrid::getVariableValue(const CState* s, const CAction* a)
{
	return s->getValue(m_hVariable.get());
}

CLASS_CONSTRUCTOR(CActionVariableGrid)
{
	m_hVariable = ACTION_VARIABLE(pParameters,"Variable", "The action variable");
	//ACTION_VARIABLE_REF(m_hVariable, "Variable", "The action variable");
	m_numCenters = INT_PARAM(pParameters, "Num-Features","The number of points that form the grid",3);
	//CONST_INTEGER_VALUE(m_numCenters, "Num-Features", 3, "The number of points that form the grid");

	m_min = CSimionApp::get()->pWorld->getDynamicModel()->getActionDescriptor()->getMin(m_hVariable.get());
	m_max = CSimionApp::get()->pWorld->getDynamicModel()->getActionDescriptor()->getMax(m_hVariable.get());

	m_distributionType = ENUM_PARAM<Distribution>(pParameters, "Distribution"
		, "The manner in which the points are distributed on the action variable's grid",Distribution::linear);
	//ENUM_VALUE(m_distributionType, Distribution,"Distribution", "linear","The manner in which the points are distributed on the action variable's grid");
	initCenterPoints();
	END_CLASS();
}

void CActionVariableGrid::setFeatureStateAction(unsigned int feature, CState* s, CState* a)
{
	a->setValue(m_hVariable.get(), m_pCenters[feature]);
}

double CActionVariableGrid::getVariableValue(const CState* s, const CAction* a)
{
	return a->getValue(m_hVariable.get());
}