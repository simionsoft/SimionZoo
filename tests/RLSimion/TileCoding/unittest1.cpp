#include "stdafx.h"
#include "CppUnitTest.h"
#include <Windows.h>
#include "../../../RLSimion/single-dimension-grid.h"
#include "../../../RLSimion/featuremap.h"
#include "../../../RLSimion/named-var-set.h"
#include "../../../RLSimion/app.h"
#include "../../../RLSimion/simgod.h"
#include "../../../RLSimion/features.h"
#include "../../../RLSimion/worlds/world.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TileCodingTest
{
	TEST_CLASS(UnitTest1)
	{
	public:

		//void test_all_low(int highDimension, TileCodingStateFeatureMap* map,
		//	FeatureList* outFeatures, MULTI_VALUE<SingleDimensionStateVariableGrid> m_grid, State* s)
		//{
		//	outFeatures->clear();

		//	//set al values to min value
		//	for (int i = 0; i < m_grid.size(); i++)
		//	{
		//		NamedVarProperties& prop = m_grid[i]->getVarProperties(s, NULL);

		//		double val = prop.getMin();
		//		s->set(prop.getName(), val);
		//		double b = s->get(prop.getName());
		//		Assert::IsTrue(val == b);
		//	}

		//	//set the one dimension to its max value
		//	NamedVarProperties& prop = m_grid[highDimension]->getVarProperties(s, NULL);
		//	double val = prop.getMax();
		//	s->set(prop.getName(), val);

		//	//get the resulting features
		//	map->getFeatures(s, outFeatures);
		//	Assert::IsTrue(outFeatures->m_numFeatures == map->getNumTilings());

		//	int expectedIndex = (m_grid[highDimension]->getNumCenters()-1);

		//	for (int i = 0; i < highDimension; i++)
		//	{
		//		expectedIndex *= (m_grid[i]->getNumCenters());
		//	}

		//	Assert::IsTrue(outFeatures->m_pFeatures[0].m_index == expectedIndex);
		//}

		TEST_METHOD(FeatureMap_TileCoding_MapUnmapSweep)
		{
			double minX = 0.0, maxX = 10.0;
			double minY = 0.0, maxY = 10.0;

			Descriptor stateDescriptor;
			size_t hX = stateDescriptor.addVariable("x", "m", minX, maxX);
			size_t hY = stateDescriptor.addVariable("y", "m", minY, maxY);
			Descriptor actionDescriptor;
			size_t hAction = stateDescriptor.addVariable("force", "N", -1.0, 1.0);

			State* s = stateDescriptor.getInstance();
			State* s_p = stateDescriptor.getInstance();

			const int numFeaturesPerTile = 10;
			const double offset = 0.05;
			const double numTiles = 5;

			TileCodingFeatureMap tileCodingFeatureMap = TileCodingFeatureMap(stateDescriptor, { hX, hY }, actionDescriptor, {}, numTiles, offset, numFeaturesPerTile);

			FeatureList* outFeatures = new FeatureList("testFeatureList");

			for (size_t iX = 0; iX < numFeaturesPerTile; iX++)
			{
				for (size_t iY = 0; iY < numFeaturesPerTile; iY++)
				{
					s->set(hX, minX + (maxX - minX) / ((double)(numFeaturesPerTile - 1)) * (double)iX);
					s->set(hY, minY + (maxY - minY) / ((double)(numFeaturesPerTile - 1)) * (double)iY);

					tileCodingFeatureMap.getFeatures(s, nullptr, outFeatures);
					size_t maxFactorFeature = outFeatures->maxFactorFeature();
					tileCodingFeatureMap.getFeatureStateAction(maxFactorFeature, s_p, nullptr);

					Assert::IsTrue(outFeatures->m_numFeatures == numTiles);
					Assert::AreEqual(s->get(hX), s_p->get(hX), 0.1, L"Incorrect behavior in GaussianRBFGrid (x)");
					Assert::AreEqual(s->get(hY), s_p->get(hY), 0.1, L"Incorrect behavior in GaussianRBFGrid (y)");
				}
			}
		}

	};
}