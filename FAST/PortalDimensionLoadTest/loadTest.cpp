#include "stdafx.h"
#include "CppUnitTest.h"
#include "../FASTDimensionalPortalDLL/FASTDimensionalPortalDLL.h"
#ifdef _DEBUG
#pragma comment(lib,"../../Debug/FASTDimensionalPortalDLL.lib")
#else
#pragma comment(lib,"../../Release/FASTDimensionalPortalDLL.lib")
#endif
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PortalDimensionLoadTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(LoadTest)
		{
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

		}

	};
}