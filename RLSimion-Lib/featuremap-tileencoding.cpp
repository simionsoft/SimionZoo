#include "stdafx.h"
#include "featuremap.h"
#include "named-var-set.h"
#include "worlds/world.h"
#include "features.h"
#include "config.h"
#include "single-dimension-grid.h"

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
	unsigned int offset = 1;

	//initialize outFeatures with the right size
	outFeatures->clear();

	if (m_grid.size() == 0) return;

	unsigned int basicTilingOffset = m_totalNumFeatures / m_numTilings;
	for (int d = 0; d < m_numTilings; d++)
	{
		unsigned int tilingOffset = basicTilingOffset*d;

		double variableOffset = m_tilingOffset * d;

		CState const* modified_s;
		CState const* modified_a;

		if (a != NULL)
		{
			CAction* temp_a = new CAction(*a);
			temp_a->addOffset(variableOffset);
			modified_a = new CAction(*temp_a);
		}
		else
			modified_a = a;

		if (s != NULL)
		{
			CState* temp_s = new CState(*s);
			temp_s->addOffset(variableOffset);
			modified_s = new CState(*temp_s);
		}
		else
			modified_s = s;

		unsigned int index = 0;
		for (int i = 0; i < m_grid.size(); i++)
		{
			m_grid[i]->getFeatures(modified_s, modified_a, m_pVarFeatures);


			//find index of the only 1.0 in the features and store it in oneIndex
			int oneIndex = -1;
			for (int j = 0; j < m_grid[i]->getNumCenters(); j++)
			{
				if (m_pVarFeatures->getFactor(j) == 1.0)
				{
					oneIndex = j;
					break;
				}
			}

			assert(oneIndex != -1);

			//calculate the product of the lengths of previous processes feature dimensions
			unsigned int prodLength = 1;

			for (int j = i-1; j >= 0; j--)
			{
				prodLength *= m_grid[j]->getNumCenters();
			}

			index += oneIndex * prodLength;
		}

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

	//pre-calculate number of features
	m_totalNumFeatures = 1;

	for (unsigned int i = 0; i < m_grid.size(); i++)
		m_totalNumFeatures *= m_grid[i]->getNumCenters();

	m_totalNumFeatures *= m_numTilings;
}

CTileCodingActionFeatureMap::CTileCodingActionFeatureMap(CConfigNode* pConfigNode)
	: CTileCodingFeatureMap(pConfigNode), CActionFeatureMap(pConfigNode)
{
	m_pVarFeatures = new CFeatureList("TileEncoding/var");

	m_grid = MULTI_VALUE<CActionVariableGrid>(pConfigNode, "Tile-Encoding-Grid-Dimension", "Parameters of the action-dimension's grid");

	//pre-calculate number of features
	m_totalNumFeatures = 1;

	for (unsigned int i = 0; i < m_grid.size(); i++)
		m_totalNumFeatures *= m_grid[i]->getNumCenters();

	m_totalNumFeatures *= m_numTilings;
}