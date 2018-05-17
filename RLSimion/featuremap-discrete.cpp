#include "featuremap.h"
#include "features.h"
#include "parameters.h"
#include "single-dimension-grid.h"

DiscreteFeatureMap::DiscreteFeatureMap()
{
}

DiscreteFeatureMap::DiscreteFeatureMap(ConfigNode* pConfigNode)
{

}

void DiscreteFeatureMap::init(vector<SingleDimensionGrid*>& grids)
{
	//calculate the number of features
	m_totalNumFeatures = 1;
	for (size_t i = 0; i < grids.size(); i++)
	{
		m_totalNumFeatures *= grids[i]->getValues().size();

	}
	m_maxNumActiveFeatures = 1;
}

DiscreteFeatureMap::~DiscreteFeatureMap()
{

}

void DiscreteFeatureMap::map(vector<SingleDimensionGrid*>& grids, const vector<double>& values, FeatureList* outFeatures)
{
	size_t offset = 1, featureIndex = 0;

	outFeatures->clear();
	if (grids.size() == 0) return;
	//features of the 0th variable in m_pBuffer
	featureIndex = grids[0]->getClosestFeature(values[0]);

	for (size_t i = 1; i < grids.size(); i++)
	{
		offset *= grids[i-1]->getValues().size();

		//we calculate the features of i-th variable
		featureIndex += offset*grids[i]->getClosestFeature(values[i]);
	}
	outFeatures->add(featureIndex, 1.0);
}


void DiscreteFeatureMap::unmap(size_t feature, vector<SingleDimensionGrid*>& grids, vector<double>& outValues)
{
	size_t dimFeature;

	for (size_t i = 0; i < grids.size(); i++)
	{
		dimFeature = feature % grids[i]->getValues().size();

		outValues[i] = grids[i]->getFeatureValue(dimFeature);

		feature = feature / grids[i]->getValues().size();
	}
}