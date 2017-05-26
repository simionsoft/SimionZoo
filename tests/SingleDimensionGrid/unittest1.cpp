#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../RLSimion-Lib/single-dimension-grid.h"
#include "../../RLSimion-Lib/named-var-set.h"
#include "../../RLSimion-Lib/app-rlsimion.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SingleDimensionGrid
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(SingleDimensionGrid_CircularVariables)
		{
			CConfigFile configFile;
			CConfigNode* pConfigNode = configFile.loadFile("..\\tests\\q-learning-test.exp");
			RLSimionApp *pApp = new RLSimionApp(pConfigNode);

		}

	};
}