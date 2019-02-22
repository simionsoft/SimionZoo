#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../../3rd-party/FAST/FASTDimensionalPortal/FASTDimensionalPortalDLL.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PortalDimensionLoadTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(FASTPortalDLL_LoadTest)
		{
#ifndef _WIN64
			// TODO: Your test code here
			try
			{
				float avrSwap[1000];
				int aviFAIL;
				char inConfigFILE []= "None";
				char outName[1000],outMsg[1000];

				avrSwap[0] = 0.0;//iStatus
				DISCON(avrSwap, &aviFAIL, inConfigFILE, outName, outMsg);
			}
			catch(std::exception ex)
			{
				Assert::Fail(L"Failed trying to load the DLL");
			}
#endif
		}

	};
}