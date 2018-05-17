#include "featuremap.h"
#include "named-var-set.h"
#include "worlds/world.h"
#include "features.h"
#include "config.h"
#include "single-dimension-grid.h"
#include "app.h"

TileCodingFeatureMap::TileCodingFeatureMap(size_t numTiles, double tileOffset)
{
	m_numTiles.set( (int) numTiles);
	m_tileOffset.set( (int) tileOffset);
}

TileCodingFeatureMap::TileCodingFeatureMap(ConfigNode* pConfigNode)
{
	m_numTiles = INT_PARAM(pConfigNode, "Num-Tiles", "Number of tile layers of the grid", 5);
	m_tileOffset = DOUBLE_PARAM(pConfigNode, "Tile-Offset", "Offset of each tile relative to the previous one. It is scaled by the value range of the input variable", 0.05);
}

void TileCodingFeatureMap::init(vector<SingleDimensionGrid*>& grids)
{
	//calculate number of features
	m_maxNumActiveFeatures = m_numTiles.get();

	m_numFeaturesPerTile = 1;

	for (unsigned int i = 0; i < grids.size(); i++)
		m_numFeaturesPerTile *= grids[i]->getValues().size();

	m_totalNumFeatures = m_numTiles.get() * m_numFeaturesPerTile;
}

TileCodingFeatureMap::~TileCodingFeatureMap()
{
}


//https://www.cs.utexas.edu/~pstone/Papers/bib2html-links/SARA05.slides.pdf
void TileCodingFeatureMap::map(vector<SingleDimensionGrid*>& grids, const vector<double>& values, FeatureList* outFeatures)
{
	//initialize outFeatures with the right size
	outFeatures->clear();

	if (grids.size() == 0) return;

	//each tile's features start from a different offset. If each tile has n features,
	//tile #0 starts from feature #0, tile #1 from feature #n, ...
	size_t tileIndexOffset = 0;

	for (size_t layerIndex = 0; layerIndex < m_numTiles.get(); layerIndex++)
	{
		//the offset by which the feature of a i is multiplied
		size_t tileDimIndexOffset = 1;
		//the index of the feature within current tile (the tile's offset will be added before adding the feature to the output)
		size_t tileFeatureIndex = 0;
		for (size_t dimension = 0; dimension < grids.size(); dimension++)
		{
			//the value offset added to the variable value
			double tileDimOffset = grids[dimension]->getRangeWidth() * m_tileOffset.get() * (double) layerIndex;
			//the value of the variable
			double variableValue = values[dimension];
			//calculate the index within the current tile using the i's offset
			tileFeatureIndex += tileDimIndexOffset * grids[dimension]->getClosestFeature(variableValue + tileDimOffset);
			//update i offset
			tileDimIndexOffset *= grids[dimension]->getValues().size();
		}
		//set the activation feature of this tile
		outFeatures->add(tileFeatureIndex + tileIndexOffset, 1.0);
		//add the number of features per tile to the tile offset. Note: must be added to keep independent value ranges: [0, n-1], [n, 2n-1]...
		tileIndexOffset += m_numFeaturesPerTile;
	}

	outFeatures->normalize();
}


void TileCodingFeatureMap::unmap(size_t feature, vector<SingleDimensionGrid*>& grids, vector<double>& outValues)
{
	//tiles overlap and there is no easy way to invert the feature map
	//as an approximation, instead of averaging the centers of every tile, we use the first i

	//calculate the feature index within the first tile
	feature = feature % m_numFeaturesPerTile;

	//now unpack each variable's value in the first tile
	for (size_t i = 0; i < grids.size(); i++)
	{
		size_t tileDimIndex = feature % grids[i]->getValues().size();

		outValues[i] = grids[i]->getValues()[tileDimIndex];

		feature = feature / grids[i]->getValues().size();
	}
}
