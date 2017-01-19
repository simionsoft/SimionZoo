
#include "../FASTDimensionalPortalDLL/FASTDimensionalPortalDLL.h"
#ifdef _DEBUG
#pragma comment(lib,"../../Debug/FASTDimensionalPortalDLL.lib")
#else
#pragma comment(lib,"../../Release/FASTDimensionalPortalDLL.lib")
#endif

void main()
{
	float avrSwap[1000];
	int aviFAIL;
	char inConfigFILE[] = "None";
	char outName[1000], outMsg[1000];

	avrSwap[0] = 0.0;//iStatus
	DISCON(avrSwap, &aviFAIL, inConfigFILE, outName, outMsg);
}