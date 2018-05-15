#include "featuremap.h"
#include "named-var-set.h"
#include "worlds/world.h"
#include "features.h"
#include "config.h"
#include "single-dimension-grid.h"
#include "app.h"

TileCodingFeatureMap::TileCodingFeatureMap(ConfigNode* pConfigNode)
	:FeatureMap(pConfigNode)
{
	m_numTiles = INT_PARAM(pConfigNode, "Num-Tiles", "Number of tile layers of the grid", 5);
	m_tileOffset = DOUBLE_PARAM(pConfigNode, "Tile-offset", "Offset of each tile relative to the previous one. It is scaled by the value range of the input variable", 0.05);
	m_numFeaturesPerTile = INT_PARAM(pConfigNode, "Features-Per-Tile", "Number of tiles per input variable", 10);

	//pre-calculate number of features
	m_maxNumActiveFeatures = m_numTiles.get();

	for (unsigned int i = 0; i < m_grids.size(); i++)
		m_totalNumFeatures *= m_numFeaturesPerTile.get();

	//state variables
	for (unsigned int i = 0; i < m_stateVariables.size(); i++)
	{
		m_grids.push_back(new SingleDimensionGrid(m_numFeaturesPerTile.get()
			, m_stateVariables[i]->getProperties().getMin(), m_stateVariables[i]->getProperties().getMax()
			, m_stateVariables[i]->getProperties().bIsCircular()));
	}
	//action variables
	for (unsigned int i = 0; i < m_actionVariables.size(); i++)
	{
		m_actionVariableNames.push_back(m_actionVariables[i]->getName());
		m_grids.push_back(new SingleDimensionGrid(m_numFeaturesPerTile.get()
			, m_actionVariables[i]->getProperties().getMin(), m_actionVariables[i]->getProperties().getMax()
			, m_actionVariables[i]->getProperties().bIsCircular()));
	}
}

TileCodingFeatureMap::~TileCodingFeatureMap()
{
}

void TileCodingFeatureMap::getFeatures(const State* s, const Action* a, FeatureList* outFeatures)
{
	//initialize outFeatures with the right size
	outFeatures->clear();

	if (m_grids.size() == 0) return;

	size_t basicTilingIndexOffset = m_totalNumFeatures / m_numTiles.get();

	for (size_t layerIndex = 0; layerIndex < m_numTiles.get(); layerIndex++)
	{
		size_t tilingIndexOffset = basicTilingIndexOffset * layerIndex;

		size_t index = tilingIndexOffset;

		for (size_t variableDimension = 0; variableDimension < m_grids.size(); variableDimension++)
		{
			//set offset of the grid
			double offset = m_grids[variableDimension]->getRangeWidth() * m_tilingOffset * (double) layerIndex;

			double variableValue = getInputVariableValue(variableDimension, s, a);
			size_t featureIndex= m_grids[variableDimension]->getClosestFeature(variableValue + offset);
			
			//calculate the index offset of the tile
			size_t prodLength = 1;

			for (size_t j = 0; j < variableDimension; j++)
				prodLength *= m_numFeaturesPerTile.get();

			index += featureIndex * prodLength;
		}

		//set the activation feature of this tile
		outFeatures->add(index, 1.0);
	}

	outFeatures->normalize();
}


void TileCodingFeatureMap::getFeatureStateAction(size_t feature, State* s, Action* a)
{
	//first determine the tile layer of the feature and subtract it to get the index within this layer
	feature = feature % (m_totalNumFeatures / m_numTiles.get());

	//now we have to unpack the feature step by step by going through each dimension
	for (size_t i = m_grids.size() - 1; i >= 0; i--)
	{
		size_t prodLength = 1;
		for (size_t j = 0; j < i; j++)
		{
			prodLength *= m_grids[j]->getValues().size();
		}
		
		getFeatureStateAction(feature / prodLength, s, a);

		if (prodLength <= feature)
			feature = feature % prodLength;
	}
}
