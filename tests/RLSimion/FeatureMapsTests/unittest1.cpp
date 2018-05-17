#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../../RLSimion/single-dimension-grid.h"
#include "../../../RLSimion/featuremap.h"
#include "../../../RLSimion/named-var-set.h"
#include "../../../RLSimion/features.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace VariableCircularity
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		TEST_METHOD(FeatureMap_RBFGrid_MapUnmapSweep)
		{
			double minX = 0.0, maxX = 10.0;
			double minY = 0.0, maxY = 10.0;

			Descriptor stateDescriptor;
			size_t hX = stateDescriptor.addVariable("x", "m", minX, maxX);
			size_t hY = stateDescriptor.addVariable("y", "m", minY, maxY);

			State* s = stateDescriptor.getInstance();
			State* s_p = stateDescriptor.getInstance();
			const int numFeatures = 10;

			StateFeatureMap rbfGrid = StateFeatureMap(new GaussianRBFGridFeatureMap(), stateDescriptor, { hX, hY }, numFeatures);

			FeatureList* outFeatures = new FeatureList("test");

			for (size_t iX = 0; iX < numFeatures; iX++)
			{
				for (size_t iY = 0; iY < numFeatures; iY++)
				{
					s->set(hX, minX + (maxX - minX) / ((double)(numFeatures - 1)) * (double)iX);
					s->set(hY, minY + (maxY - minY) / ((double)(numFeatures - 1)) * (double)iY);

					rbfGrid.getFeatures(s, nullptr, outFeatures);
					size_t maxFactorFeature = outFeatures->maxFactorFeature();
					rbfGrid.getFeatureStateAction(maxFactorFeature, s_p, nullptr);

					Assert::AreEqual(s->get(hX), s_p->get(hX), 0.1, L"Incorrect behavior in GaussianRBFGrid (x)");
					Assert::AreEqual(s->get(hY), s_p->get(hY), 0.1, L"Incorrect behavior in GaussianRBFGrid (y)");
				}
			}
			delete s;
			delete s_p;

		}
		TEST_METHOD(FeatureMap_RBFGrid_VariableCircularity)
		{
			Descriptor stateDescriptor;
			size_t hState1 = stateDescriptor.addVariable("angle", "rad", -3.1415, 3.1415, true);
			size_t hState2 = stateDescriptor.addVariable("angle2", "rad", -3.1415, 3.1415);

			State* s= stateDescriptor.getInstance();
			const int numFeatures = 10;

			StateFeatureMap gridState1 = StateFeatureMap(new GaussianRBFGridFeatureMap(), stateDescriptor, { hState1 }, numFeatures);

			FeatureList* pFeatures = new FeatureList("test");

			s->set("angle", -3.14);

			gridState1.getFeatures(s, nullptr, pFeatures);
			Assert::IsTrue(pFeatures->getFeaturePos(numFeatures - 1) >= 0);
			Assert::IsTrue(pFeatures->maxFactorFeature() == 0);

			s->set("angle", 3.14);

			gridState1.getFeatures(s, nullptr, pFeatures);
			Assert::IsTrue(pFeatures->getFeaturePos(0) >= 0);
			Assert::IsTrue(pFeatures->maxFactorFeature() == 0);

			//test the non-circular variable
			StateFeatureMap gridState2 = StateFeatureMap(new GaussianRBFGridFeatureMap(), stateDescriptor, { hState2 }, numFeatures);

			s->set("angle2", 3.14);
			gridState2.getFeatures(s, 0, pFeatures);
			Assert::IsTrue(pFeatures->getFeaturePos(0) == -1);
			Assert::IsTrue(pFeatures->maxFactorFeature() == numFeatures-1);

			s->set("angle2", -3.14);
			gridState2.getFeatures(s, 0, pFeatures);
			Assert::IsTrue(pFeatures->getFeaturePos(numFeatures - 1) ==-1);
			Assert::IsTrue(pFeatures->maxFactorFeature() == 0);
		}
		TEST_METHOD(FeatureMap_Discrete_MapUnmapSweep)
		{
			double minX = 0.0, maxX = 10.0;
			double minY = 0.0, maxY = 10.0;

			Descriptor stateDescriptor;
			size_t hX = stateDescriptor.addVariable("x", "m", minX, maxX);
			size_t hY = stateDescriptor.addVariable("y", "m", minY, maxY);

			State* s = stateDescriptor.getInstance();
			State* s_p = stateDescriptor.getInstance();
			const int numFeatures = 10;

			StateFeatureMap featureMap = StateFeatureMap(new DiscreteFeatureMap(), stateDescriptor, { hX, hY }, numFeatures);

			FeatureList* outFeatures = new FeatureList("test");

			for (size_t iX = 0; iX < numFeatures; iX++)
			{
				for (size_t iY = 0; iY < numFeatures; iY++)
				{
					s->set(hX, minX + (maxX - minX) / ((double)(numFeatures - 1)) * (double)iX);
					s->set(hY, minY + (maxY - minY) / ((double)(numFeatures - 1)) * (double)iY);

					featureMap.getFeatures(s, nullptr, outFeatures);
					size_t maxFactorFeature = outFeatures->maxFactorFeature();
					featureMap.getFeatureStateAction(maxFactorFeature, s_p, nullptr);

					Assert::AreEqual(s->get(hX), s_p->get(hX), 0.1, L"Incorrect behavior in DiscreteFeatureMap (x)");
					Assert::AreEqual(s->get(hY), s_p->get(hY), 0.1, L"Incorrect behavior in DiscreteFeatureMap (y)");
				}
			}
			delete s;
			delete s_p;
		}
		TEST_METHOD(FeatureMap_Discrete_VariableCircularity)
		{
			Descriptor stateDescriptor;
			size_t hState1 = stateDescriptor.addVariable("angle", "rad", -3.1415, 3.1415, true);
			size_t hState2 = stateDescriptor.addVariable("angle2", "rad", -3.1415, 3.1415);

			State* s = stateDescriptor.getInstance();
			const int numFeatures = 10;

			StateFeatureMap gridState1 = StateFeatureMap(new DiscreteFeatureMap(), stateDescriptor, { hState1 }, numFeatures);

			FeatureList* pFeatures = new FeatureList("test");

			s->set("angle", -3.14);

			gridState1.getFeatures(s, nullptr, pFeatures);
			Assert::IsTrue(pFeatures->maxFactorFeature() == 0);

			s->set("angle", 3.14);

			gridState1.getFeatures(s, nullptr, pFeatures);
			Assert::IsTrue(pFeatures->getFeaturePos(0) >= 0);
			Assert::IsTrue(pFeatures->maxFactorFeature() == 0);

			//test the non-circular variable
			StateFeatureMap gridState2 = StateFeatureMap(new DiscreteFeatureMap(), stateDescriptor, { hState2 }, numFeatures);

			s->set("angle2", -3.14);
			gridState2.getFeatures(s, 0, pFeatures);
			Assert::IsTrue(pFeatures->getFeaturePos(numFeatures - 1) == -1);
			Assert::IsTrue(pFeatures->maxFactorFeature() == 0);

			s->set("angle2", 3.14);
			gridState2.getFeatures(s, 0, pFeatures);
			Assert::IsTrue(pFeatures->getFeaturePos(0) == -1);
			Assert::IsTrue(pFeatures->maxFactorFeature() == numFeatures - 1);
		}

		TEST_METHOD(FeatureMap_TileCoding_MapUnmapSweep)
		{
			double minX = 0.0, maxX = 10.0;
			double minY = 0.0, maxY = 10.0;

			Descriptor stateDescriptor;
			size_t hX = stateDescriptor.addVariable("x", "m", minX, maxX);
			size_t hY = stateDescriptor.addVariable("y", "m", minY, maxY);

			State* s = stateDescriptor.getInstance();
			State* s_p = stateDescriptor.getInstance();

			const size_t numFeaturesPerTile = 10;
			const double offset = 0.05;
			const size_t numTiles = 5;

			StateFeatureMap tileCodingFeatureMap = StateFeatureMap(new TileCodingFeatureMap(numTiles, offset), stateDescriptor, { hX, hY }, numFeaturesPerTile);

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