#include "featuremap.h"
#include "named-var-set.h"
#include "worlds/world.h"
#include "features.h"
#include "config.h"
#include "single-dimension-grid.h"
#include "app-rlsimion.h"

template <typename dimensionGridType>
TileCodingFeatureMap<dimensionGridType>::TileCodingFeatureMap(ConfigNode* pParameters)
{
}

template <typename dimensionGridType>
TileCodingFeatureMap<dimensionGridType>::~TileCodingFeatureMap()
{
	delete m_pVarFeatures;
}

template <typename dimensionGridType>
void TileCodingFeatureMap<dimensionGridType>::getFeatures(const State* s, const Action* a, FeatureList* outFeatures)
{
	//initialize outFeatures with the right size
	outFeatures->clear();

	if (m_grid.size() == 0) return;

	size_t basicTilingIndexOffset = m_totalNumFeatures / m_numTilings;
	for (size_t layerIndex = 0; layerIndex < m_numTilings; layerIndex++)
	{
		size_t tilingIndexOffset = basicTilingIndexOffset*layerIndex;

		size_t index = tilingIndexOffset;
		for (size_t variableDimension = 0; variableDimension < m_grid.size(); variableDimension++)
		{
			//set offset of the grid
			double oldOffset = m_grid[variableDimension]->getOffset();
			m_grid[variableDimension]->setOffset(m_tilingOffset * layerIndex + oldOffset);

			m_pVarFeatures->clear();
			m_grid[variableDimension]->getFeatures(s, a, m_pVarFeatures);

			//find index of the only 1.0 in the features and store it in oneIndex
			size_t oneIndex = m_pVarFeatures->m_pFeatures[0].m_index;

			//calculate the product of the lengths of previous processes feature dimensions
			size_t prodLength = 1;

			for (size_t j = 0; j < variableDimension; j++)
			{
				prodLength *= m_grid[j]->getNumCenters();
			}

			index += oneIndex * prodLength;

			//reset offset
			m_grid[variableDimension]->setOffset(oldOffset);
		}

		//set the activated feature of this tiling layer
		outFeatures->add((unsigned int)index, 1.0);
	}

	outFeatures->normalize();
}

template <typename dimensionGridType>
void TileCodingFeatureMap<dimensionGridType>::getFeatureStateAction(unsigned int feature, State* s, Action* a)
{
	//at first: determine the tile layer of the feature and subtract it to get the index within this layer
	feature = feature % (m_totalNumFeatures / m_numTilings);

	//now we have to unpack the feature step by step by going through each dimension
	for (size_t i = m_grid.size() - 1; i >= 0; i--)
	{
		unsigned int prodLength = 1;
		for (int j = 0; j < i; j++)
		{
			prodLength *= m_grid[j]->getNumCenters();
		}
		
		m_grid[i]->setFeatureStateAction(feature / prodLength, s, a);
		if (prodLength <= feature)
			feature = feature % prodLength;
	}
}

TileCodingStateFeatureMap::TileCodingStateFeatureMap(ConfigNode* pConfigNode)
	: TileCodingFeatureMap(pConfigNode), StateFeatureMap(pConfigNode)
{
	m_pVarFeatures = new FeatureList("TileEncoding/var");

	m_grid = MULTI_VALUE<SingleDimensionStateVariableGrid>(pConfigNode, "Tile-Encoding-Grid-Dimension", "Parameters of the state-dimension's grid");

	m_numTilings = INT_PARAM(pConfigNode, "Tile-layers", "Number of tile layers of the grid", 1).get();
	m_tilingOffset = DOUBLE_PARAM(pConfigNode, "Tile-offset", "Offset of each tile relative to the previous one", 0.0).get();

	//pre-calculate number of features
	m_totalNumFeatures = m_numTilings;

	for (unsigned int i = 0; i < m_grid.size(); i++)
	{
		m_totalNumFeatures *= m_grid[i]->getNumCenters();
	}
}

TileCodingActionFeatureMap::TileCodingActionFeatureMap(ConfigNode* pConfigNode)
	: TileCodingFeatureMap(pConfigNode), ActionFeatureMap(pConfigNode)
{
	m_pVarFeatures = new FeatureList("TileEncoding/var");

	m_grid = MULTI_VALUE<SingleDimensionActionVariableGrid>(pConfigNode, "Tile-Encoding-Grid-Dimension", "Parameters of the action-dimension's grid");

	m_numTilings = INT_PARAM(pConfigNode, "Tile-layers", "Number of tile layers of the grid", 1).get();
	m_tilingOffset = DOUBLE_PARAM(pConfigNode, "Tile-offset", "Offset of each tile relative to the previous one", 0.0).get();

	//pre-calculate number of features
	m_totalNumFeatures = m_numTilings;

	for (unsigned int i = 0; i < m_grid.size(); i++)
	{
		m_totalNumFeatures *= m_grid[i]->getNumCenters();
	}
}