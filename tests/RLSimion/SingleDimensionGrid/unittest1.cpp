#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../../RLSimion-Lib/single-dimension-grid.h"
#include "../../../RLSimion-Lib/named-var-set.h"
#include "../../../RLSimion-Lib/app-rlsimion.h"
#include "../../../RLSimion-Lib/features.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
#include <Windows.h>
namespace SingleDimensionGrid
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(SingleDimensionGrid_CircularVariables)
		{
			DWORD buffer[512];
			GetCurrentDirectory(512,(LPWSTR)buffer);
			CConfigFile configFile;
			CConfigNode* pConfigNode = configFile.loadFile("..\\tests\\q-learning-test.simion.exp");
			RLSimionApp *pApp = new RLSimionApp(pConfigNode);
			int hVar= pApp->pWorld->getDynamicModel()->addStateVariable("test-angle", "rad", -3.1415, 3.1415);
			int hVar2= pApp->pWorld->getDynamicModel()->addStateVariable("test-angle2", "rad", -3.1415, 3.1415);
			CDescriptor& descr= pApp->pWorld->getDynamicModel()->getStateDescriptor();
			descr[hVar].setCircular(true);

			CState* s= pApp->pWorld->getDynamicModel()->getStateInstance();
			const int numFeatures = 10;
			CStateVariableGrid gridVar1(hVar,numFeatures);
			CFeatureList* pFeatures = new CFeatureList("test");

			s->set("test-angle", -3.14);

			gridVar1.getFeatures(s, 0, pFeatures);
			Assert::IsTrue(pFeatures->m_pFeatures[0].m_index == numFeatures - 1
				|| pFeatures->m_pFeatures[1].m_index == numFeatures - 1
				|| pFeatures->m_pFeatures[2].m_index == numFeatures - 1);

			s->set("test-angle", 3.14);

			gridVar1.getFeatures(s, 0, pFeatures);
			Assert::IsTrue(pFeatures->m_pFeatures[0].m_index == 0
				|| pFeatures->m_pFeatures[1].m_index == 0
				|| pFeatures->m_pFeatures[2].m_index == 0);

			//test the non-circular variable
			CStateVariableGridRBF gridVar2(hVar2, numFeatures);
			s->set("test-angle2", 3.14);
			gridVar2.getFeatures(s, 0, pFeatures);
			Assert::IsFalse(pFeatures->m_pFeatures[0].m_index == 0
				|| pFeatures->m_pFeatures[1].m_index == 0
				|| pFeatures->m_pFeatures[2].m_index == 0);
			s->set("test-angle2", -3.14);
			gridVar2.getFeatures(s, 0, pFeatures);
			Assert::IsFalse(pFeatures->m_pFeatures[0].m_index == numFeatures - 1
				|| pFeatures->m_pFeatures[1].m_index == numFeatures - 1
				|| pFeatures->m_pFeatures[2].m_index == numFeatures - 1);
		}

	};
}