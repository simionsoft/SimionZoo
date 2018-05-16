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
			Descriptor actionDescriptor;
			size_t hAction = stateDescriptor.addVariable("force", "N", -1.0, 1.0);

			State* s = stateDescriptor.getInstance();
			State* s_p = stateDescriptor.getInstance();
			const int numFeatures = 10;

			GaussianRBFGridFeatureMap rbfGrid = GaussianRBFGridFeatureMap(stateDescriptor, { hX, hY }, actionDescriptor, {}, numFeatures);

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
			Descriptor actionDescriptor;
			size_t hAction = stateDescriptor.addVariable("force", "N", -1.0, 1.0);
			

			State* s= stateDescriptor.getInstance();
			const int numFeatures = 10;

			GaussianRBFGridFeatureMap gridState1 = GaussianRBFGridFeatureMap(stateDescriptor, { hState1 }, actionDescriptor, {}, numFeatures);

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
			GaussianRBFGridFeatureMap gridState2 = GaussianRBFGridFeatureMap(stateDescriptor, { hState2 }, actionDescriptor, {}, numFeatures);

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
			Descriptor actionDescriptor;
			size_t hAction = stateDescriptor.addVariable("force", "N", -1.0, 1.0);

			State* s = stateDescriptor.getInstance();
			State* s_p = stateDescriptor.getInstance();
			const int numFeatures = 10;

			DiscreteFeatureMap rbfGrid = DiscreteFeatureMap(stateDescriptor, { hX, hY }, actionDescriptor, {}, numFeatures);

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
			Descriptor actionDescriptor;
			size_t hAction = stateDescriptor.addVariable("force", "N", -1.0, 1.0);


			State* s = stateDescriptor.getInstance();
			const int numFeatures = 10;

			DiscreteFeatureMap gridState1 = DiscreteFeatureMap(stateDescriptor, { hState1 }, actionDescriptor, {}, numFeatures);

			FeatureList* pFeatures = new FeatureList("test");

			s->set("angle", -3.14);

			gridState1.getFeatures(s, nullptr, pFeatures);
			Assert::IsTrue(pFeatures->maxFactorFeature() == 0);

			s->set("angle", 3.14);

			gridState1.getFeatures(s, nullptr, pFeatures);
			Assert::IsTrue(pFeatures->getFeaturePos(0) >= 0);
			Assert::IsTrue(pFeatures->maxFactorFeature() == 0);

			//test the non-circular variable
			DiscreteFeatureMap gridState2 = DiscreteFeatureMap(stateDescriptor, { hState2 }, actionDescriptor, {}, numFeatures);

			s->set("angle2", -3.14);
			gridState2.getFeatures(s, 0, pFeatures);
			Assert::IsTrue(pFeatures->getFeaturePos(numFeatures - 1) == -1);
			Assert::IsTrue(pFeatures->maxFactorFeature() == 0);

			s->set("angle2", 3.14);
			gridState2.getFeatures(s, 0, pFeatures);
			Assert::IsTrue(pFeatures->getFeaturePos(0) == -1);
			Assert::IsTrue(pFeatures->maxFactorFeature() == numFeatures - 1);
		}
	};
}