#include "featuremap.h"
#include "named-var-set.h"
#include "worlds/world.h"
#include "features.h"
#include "config.h"
#include "single-dimension-grid.h"

#define ACTIVATION_THRESHOLD 0.0001

template <typename dimensionGridType>
DiscreteFeatureMap<dimensionGridType>::DiscreteFeatureMap(ConfigNode* pParameters)
{
}


template <typename dimensionGridType>
DiscreteFeatureMap<dimensionGridType>::~DiscreteFeatureMap()
{
	delete m_pVarFeatures;
}

template <typename dimensionGridType>
void DiscreteFeatureMap<dimensionGridType>::getFeatures(const State* s, const Action* a, FeatureList* outFeatures)
{
	unsigned int offset = 1;

	outFeatures->clear();
	if (m_grid.size() == 0) return;
	//features of the 0th variable in m_pBuffer
	m_grid[0]->getFeatures(s, a, outFeatures);

	for (unsigned int i = 1; i < m_grid.size(); i++)
	{
		offset *= m_grid[i - 1]->getNumCenters();

		//we calculate the features of i-th variable
		m_grid[i]->getFeatures(s, a, m_pVarFeatures);
		//spawn features in buffer with the i-th variable's features
		outFeatures->spawn(m_pVarFeatures, offset);
	}
	outFeatures->applyThreshold(ACTIVATION_THRESHOLD);
	outFeatures->normalize();
}

template <typename dimensionGridType>
void DiscreteFeatureMap<dimensionGridType>::getFeatureStateAction(unsigned int feature, State* s, Action* a)
{
	unsigned int dimFeature;

	for (unsigned int i = 0; i < m_grid.size(); i++)
	{
		dimFeature = feature % m_grid[i]->getNumCenters();

		m_grid[i]->setFeatureStateAction(dimFeature, s, a);

		feature = feature / m_grid[i]->getNumCenters();
	}
}

DiscreteStateFeatureMap::DiscreteStateFeatureMap(ConfigNode* pConfigNode)
	: DiscreteFeatureMap(pConfigNode), StateFeatureMap(pConfigNode)
{
	m_pVarFeatures = new FeatureList("LinearGrid/var");

	m_grid = MULTI_VALUE<SingleDimensionDiscreteStateVariableGrid>(pConfigNode, "Grid-Dimension", "Parameters of the state-dimension's grid");

	//pre-calculate number of features
	m_totalNumFeatures = 1;

	for (unsigned int i = 0; i < m_grid.size(); i++)
		m_totalNumFeatures *= m_grid[i]->getNumCenters();

	m_maxNumActiveFeatures = 1;
}

DiscreteActionFeatureMap::DiscreteActionFeatureMap(ConfigNode* pConfigNode)
	: DiscreteFeatureMap(pConfigNode), ActionFeatureMap(pConfigNode)
{
	m_pVarFeatures = new FeatureList("LinearGrid/var");

	m_grid = MULTI_VALUE<SingleDimensionDiscreteActionVariableGrid>(pConfigNode, "Grid-Dimension", "Parameters of the action-dimension's grid");

	//pre-calculate number of features
	m_totalNumFeatures = 1;

	for (unsigned int i = 0; i < m_grid.size(); i++)
		m_totalNumFeatures *= m_grid[i]->getNumCenters();

	m_maxNumActiveFeatures = 1;
}