/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

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

/// <summary>
/// Implements a feature mapping function that maps state-actions to boxes. Only one feature will be active
/// </summary>
/// <param name="grids">Input grids for every state-variable used</param>
/// <param name="values">The values of every state-variable used</param>
/// <param name="outFeatures">The output list of features</param>
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

/// <summary>
/// Inverse of the feature mapping operation. Given a feature it returns the state-action to which it corresponds.
/// </summary>
/// <param name="feature">The index of the feature</param>
/// <param name="grids">The set of grids used to discretize each variable</param>
/// <param name="outValues">The set of output values for every state-action variable</param>
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