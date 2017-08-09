#include "stdafx.h"
#include "featuremap.h"
#include "named-var-set.h"
#include "worlds/world.h"
#include "features.h"
#include "config.h"
#include "single-dimension-grid.h"
#include "app-rlsimion.h"

template <typename dimensionGridType>
CTileCodingFeatureMap<dimensionGridType>::CTileCodingFeatureMap(CConfigNode* pParameters)
{
}

template <typename dimensionGridType>
CTileCodingFeatureMap<dimensionGridType>::~CTileCodingFeatureMap()
{
	delete m_pVarFeatures;
}

template <typename dimensionGridType>
void CTileCodingFeatureMap<dimensionGridType>::getFeatures(const CState* s, const CAction* a, CFeatureList* outFeatures)
{
	//initialize outFeatures with the right size
	outFeatures->clear();

	if (m_grid.size() == 0) return;

	unsigned int basicTilingIndexOffset = m_totalNumFeatures / m_numTilings;
	for (int layerIndex = 0; layerIndex < m_numTilings; layerIndex++)
	{
		unsigned int tilingIndexOffset = basicTilingIndexOffset*layerIndex;

		unsigned int index = tilingIndexOffset;
		for (int variableDimension = 0; variableDimension < m_grid.size(); variableDimension++)
		{
			m_pVarFeatures->clear();
			m_grid[variableDimension]->getFeatures(s, a, m_pVarFeatures);
			
			//find index of the only 1.0 in the features and store it in oneIndex
			unsigned int oneIndex = m_pVarFeatures->m_pFeatures[0].m_index;

			//calculate the product of the lengths of previous processes feature dimensions
			unsigned int prodLength = 1;

			for (int j = 0; j < variableDimension; j++)
			{
				prodLength *= m_grid[j]->getNumCenters();
			}

			index += oneIndex * prodLength;
		}
		
		//set the activated feature of this tiling layer
		outFeatures->add(index, 1.0);
	}

	outFeatures->normalize();
}

template <typename dimensionGridType>
void CTileCodingFeatureMap<dimensionGridType>::getFeatureStateAction(unsigned int feature, CState* s, CAction* a)
{
	throw "CTileCodingFeatureMap<dimensionGridType>::getFeatureStateAction is not implemented yet.";
}

CTileCodingStateFeatureMap::CTileCodingStateFeatureMap(CConfigNode* pConfigNode)
	: CTileCodingFeatureMap(pConfigNode), CStateFeatureMap(pConfigNode)
{
	m_pVarFeatures = new CFeatureList("TileEncoding/var");

	m_grid = MULTI_VALUE<CStateVariableGrid>(pConfigNode, "Tile-Encoding-Grid-Dimension", "Parameters of the state-dimension's grid");

	m_numTilings = INT_PARAM(pConfigNode, "Tile-layers", "Number of tile layers of the grid", 1).get();
	m_tilingOffset = DOUBLE_PARAM(pConfigNode, "Tile-offset", "Offset of each tile relative to the previous one", 0.0).get();

	//pre-calculate number of features
	m_totalNumFeatures = m_numTilings;

	for (unsigned int i = 0; i < m_grid.size(); i++)
		m_totalNumFeatures *= m_grid[i]->getNumCenters();

	m_totalNumFeatures *= m_numTilings;
}

CTileCodingActionFeatureMap::CTileCodingActionFeatureMap(CConfigNode* pConfigNode)
	: CTileCodingFeatureMap(pConfigNode), CActionFeatureMap(pConfigNode)
{
	m_pVarFeatures = new CFeatureList("TileEncoding/var");

	m_grid = MULTI_VALUE<CActionVariableGrid>(pConfigNode, "Tile-Encoding-Grid-Dimension", "Parameters of the action-dimension's grid");

	m_numTilings = INT_PARAM(pConfigNode, "Tile-layers", "Number of tile layers of the grid", 1).get();
	m_tilingOffset = DOUBLE_PARAM(pConfigNode, "Tile-offset", "Offset of each tile relative to the previous one", 0.0).get();
	
	//pre-calculate number of features
	m_totalNumFeatures = m_numTilings;

	for (unsigned int i = 0; i < m_grid.size(); i++)
	{
		m_totalNumFeatures *= m_grid[i]->getNumCenters();

		m_grid[i]->setOffset(m_tilingOffset * i);
	}

	m_totalNumFeatures *= m_numTilings;
}