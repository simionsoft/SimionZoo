#include "featuremap.h"
#include "named-var-set.h"
#include "worlds/world.h"
#include "features.h"
#include "config.h"
#include "single-dimension-grid.h"
#include "app.h"

TileCodingFeatureMap::TileCodingFeatureMap(Descriptor& stateDescriptor, vector<size_t> stateVariableIds, Descriptor& actionDescriptor, vector<size_t> actionVariableIds
	, size_t numTiles, double tileOffset, size_t numFeaturesPerVariable)
{
	m_numTiles.set( (int) numTiles);
	m_tileOffset.set( (int) tileOffset);
	m_numFeaturesPerDimension.set( (int) numFeaturesPerVariable);

	//create STATE_VARIABLE objects
	for each (size_t varId in stateVariableIds)
		m_stateVariables.add(new STATE_VARIABLE(stateDescriptor, varId));
	//create ACTION_VARIABLE objects
	for each (size_t varId in actionVariableIds)
		m_actionVariables.add(new ACTION_VARIABLE(actionDescriptor, varId));

	initGrid();
}

TileCodingFeatureMap::TileCodingFeatureMap(ConfigNode* pConfigNode)
	:FeatureMap(pConfigNode)
{
	m_numTiles = INT_PARAM(pConfigNode, "Num-Tiles", "Number of tile layers of the grid", 5);
	m_tileOffset = DOUBLE_PARAM(pConfigNode, "Tile-tileDimOffset", "Offset of each tile relative to the previous one. It is scaled by the value range of the input variable", 0.05);
	m_numFeaturesPerDimension = INT_PARAM(pConfigNode, "Features-Per-Dimension", "Number of feature per input variable", 10);

	initGrid();
}

void TileCodingFeatureMap::initGrid()
{
	//pre-calculate number of features
	m_maxNumActiveFeatures = m_numTiles.get();

	m_numFeaturesPerTile = 1;

	//state variables
	for (unsigned int i = 0; i < m_stateVariables.size(); i++)
	{
		m_grids.push_back(new SingleDimensionGrid(m_numFeaturesPerDimension.get()
			, m_stateVariables[i]->getProperties()->getMin(), m_stateVariables[i]->getProperties()->getMax()
			, m_stateVariables[i]->getProperties()->isCircular()));
		m_numFeaturesPerTile *= m_numFeaturesPerDimension.get();
	}
	//action variables
	for (unsigned int i = 0; i < m_actionVariables.size(); i++)
	{
		m_grids.push_back(new SingleDimensionGrid(m_numFeaturesPerDimension.get()
			, m_actionVariables[i]->getProperties()->getMin(), m_actionVariables[i]->getProperties()->getMax()
			, m_actionVariables[i]->getProperties()->isCircular()));
		m_numFeaturesPerTile *= m_numFeaturesPerDimension.get();
	}

	m_totalNumFeatures = m_numTiles.get() * m_numFeaturesPerTile;
}

TileCodingFeatureMap::~TileCodingFeatureMap()
{
}

void TileCodingFeatureMap::getFeatures(const State* s, const Action* a, FeatureList* outFeatures)
{
	//initialize outFeatures with the right size
	outFeatures->clear();

	if (m_grids.size() == 0) return;

	//each tile's features start from a different offset. If each tile has n features,
	//tile #0 starts from feature #0, tile #1 from feature #n, ...
	size_t tileIndexOffset = 0;

	for (size_t layerIndex = 0; layerIndex < m_numTiles.get(); layerIndex++)
	{
		//the offset by which the feature of a dimension is multiplied
		size_t tileDimIndexOffset = 1;
		//the index of the feature within current tile (the tile's offset will be added before adding the feature to the output)
		size_t tileFeatureIndex = 0;
		for (size_t dimension = 0; dimension < m_grids.size(); dimension++)
		{
			//the value offset added to the variable value
			double tileDimOffset = m_grids[dimension]->getRangeWidth() * m_tileOffset.get() * (double) layerIndex;
			//the value of the variable
			double variableValue = getInputVariableValue(dimension, s, a);
			//calculate the index within the current tile using the dimension's offset
			tileFeatureIndex += tileDimIndexOffset * m_grids[dimension]->getClosestFeature(variableValue + tileDimOffset);
			//update dimension offset
			tileDimIndexOffset *= m_numFeaturesPerDimension.get();
		}
		//set the activation feature of this tile
		outFeatures->add(tileFeatureIndex + tileIndexOffset, 1.0);
		//add the number of features per tile to the tile offset. Note: must be added to keep independent value ranges: [0, n-1], [n, 2n-1]...
		tileIndexOffset += m_numFeaturesPerTile;
	}

	outFeatures->normalize();
}


void TileCodingFeatureMap::getFeatureStateAction(size_t feature, State* s, Action* a)
{
	//tiles overlap and there is no easy way to invert the feature map
	//as an approximation, instead of averaging the centers of every tile, we use the first dimension

	//calculate the feature index within the first tile
	feature = feature % m_numFeaturesPerTile;

	//now unpack each variable's value in the first tile
	for (size_t dimension = 0; dimension < m_grids.size(); dimension++)
	{
		size_t tileDimIndex = feature % m_numFeaturesPerDimension.get();

		double dimensionValue = m_grids[dimension]->getValues()[tileDimIndex];

		setInputVariableValue(dimension, dimensionValue, s, a);

		feature = feature / m_numFeaturesPerDimension.get();
	}
}
