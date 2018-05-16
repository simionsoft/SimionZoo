#include "featuremap.h"
#include "features.h"
#include "parameters.h"
#include "single-dimension-grid.h"

DiscreteFeatureMap::DiscreteFeatureMap(Descriptor& stateDescriptor, vector<size_t> stateVariableIds, Descriptor& actionDescriptor, vector<size_t> actionVariableIds, size_t numFeaturesPerVariable)
{
	m_numFeaturesPerVariable.set(numFeaturesPerVariable);

	//create STATE_VARIABLE objects
	for each (size_t varId in stateVariableIds)
		m_stateVariables.add(new STATE_VARIABLE(stateDescriptor, varId));
	//create ACTION_VARIABLE objects
	for each (size_t varId in actionVariableIds)
		m_actionVariables.add(new ACTION_VARIABLE(actionDescriptor, varId));
}

DiscreteFeatureMap::DiscreteFeatureMap(ConfigNode* pConfigNode)
	:FeatureMap(pConfigNode)
{
	m_numFeaturesPerVariable = INT_PARAM(pConfigNode, "Num-Features-Per-Dimension", "Number of features per input variable", 20);

	initGrid();
}

void DiscreteFeatureMap::initGrid()
{
	m_totalNumFeatures = 1;
	//state variables
	for (size_t i = 0; i < m_stateVariables.size(); i++)
	{
		m_totalNumFeatures *= m_numFeaturesPerVariable.get();

		m_grids.push_back(new SingleDimensionGrid(m_numFeaturesPerVariable.get()
			, m_stateVariables[i]->getProperties()->getMin(), m_stateVariables[i]->getProperties()->getMax()
			, m_stateVariables[i]->getProperties()->isCircular()));
	}
	//action variables
	for (size_t i = 0; i < m_actionVariables.size(); i++)
	{
		m_totalNumFeatures *= m_numFeaturesPerVariable.get();

		m_grids.push_back(new SingleDimensionGrid(m_numFeaturesPerVariable.get()
			, m_stateVariables[i]->getProperties()->getMin(), m_stateVariables[i]->getProperties()->getMax()
			, m_stateVariables[i]->getProperties()->isCircular()));
	}
	m_maxNumActiveFeatures = 1;
}

DiscreteFeatureMap::~DiscreteFeatureMap()
{

}

void DiscreteFeatureMap::getFeatures(const State* s, const Action* a, FeatureList* outFeatures)
{
	size_t offset = 1, featureIndex;

	outFeatures->clear();
	if (m_grids.size() == 0) return;
	//features of the 0th variable in m_pBuffer
	featureIndex = m_grids[0]->getClosestFeature(getInputVariableValue(0, s, a));

	for (size_t i = 1; i < m_grids.size(); i++)
	{
		offset *= m_numFeaturesPerVariable.get();

		//we calculate the features of i-th variable
		featureIndex += m_grids[i]->getClosestFeature(getInputVariableValue(i, s, a));
	}
	outFeatures->add(featureIndex, 1.0);
}


void DiscreteFeatureMap::getFeatureStateAction(size_t feature, State* s, Action* a)
{
	size_t dimFeature;

	for (size_t i = 0; i < m_grids.size(); i++)
	{
		dimFeature = feature % m_numFeaturesPerVariable.get();

		setInputVariableValue(i, m_grids[i]->getFeatureValue(dimFeature), s, a);

		feature = feature / m_grids.size();
	}
}