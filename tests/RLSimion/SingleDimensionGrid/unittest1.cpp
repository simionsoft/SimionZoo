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
			CStateVariableGrid grid(hVar,10);
			CFeatureList* pFeatures = new CFeatureList("test");

			s->set("test-angle", -3.14);
			s->set("test-angle2", 3.14);
			grid.getFeatures(s, 0, pFeatures);
		}

	};
}