#include "stdafx.h"
#include "CppUnitTest.h"
#include <Windows.h>
#include "../../../RLSimion/single-dimension-grid.h"
#include "../../../RLSimion/featuremap.h"
#include "../../../RLSimion/named-var-set.h"
#include "../../../RLSimion/app.h"
#include "../../../RLSimion/features.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TileCodingTest
{
	TEST_CLASS(UnitTest1)
	{
	public:

		void test_all_low(int highDimension, TileCodingStateFeatureMap* map,
			FeatureList* outFeatureList, MULTI_VALUE<SingleDimensionStateVariableGrid> m_grid, State* s)
		{
			outFeatureList->clear();

			//set al values to min value
			for (int i = 0; i < m_grid.size(); i++)
			{
				NamedVarProperties& prop = m_grid[i]->getVarProperties(s, NULL);

				double val = prop.getMin();
				s->set(prop.getName(), val);
				double b = s->get(prop.getName());
				Assert::IsTrue(val == b);
			}

			//set the one dimension to its max value
			NamedVarProperties& prop = m_grid[highDimension]->getVarProperties(s, NULL);
			double val = prop.getMax();
			s->set(prop.getName(), val);

			//get the resulting features
			map->getFeatures(s, outFeatureList);
			Assert::IsTrue(outFeatureList->m_numFeatures == map->getNumTilings());

			int expectedIndex = (m_grid[highDimension]->getNumCenters()-1);

			for (int i = 0; i < highDimension; i++)
			{
				expectedIndex *= (m_grid[i]->getNumCenters());
			}

			Assert::IsTrue(outFeatureList->m_pFeatures[0].m_index == expectedIndex);
		}

		TEST_METHOD(TileCoding_Test)
		{
			DWORD buffer[512];
			GetCurrentDirectory(512, (LPWSTR)buffer);
			ConfigFile configFile;
			ConfigNode* pConfigNode = configFile.loadFile("..\\tests\\pull-box-1.simion.exp");
			SimionApp *pApp = new SimionApp(pConfigNode);

			State* s = pApp->pWorld->getDynamicModel()->getStateInstance();
			Action* a = pApp->pWorld->getDynamicModel()->getActionInstance();

			StateFeatureMap* rawMap = pApp->pSimGod->getGlobalStateFeatureMap().get();
			TileCodingStateFeatureMap* map = (TileCodingStateFeatureMap*)rawMap;

			FeatureList* outFeatureList = new FeatureList("testFeatureList");

			MULTI_VALUE<SingleDimensionStateVariableGrid> m_grid = map->returnGrid();

			for (int i = 0; i < m_grid.size(); i++)
			{
				test_all_low(i, map, outFeatureList, m_grid, s);
			}
		}

	};
}