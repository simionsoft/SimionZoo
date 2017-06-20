// memPoolTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../../../RLSimion-Lib/mem-manager.h"
#include "../../../RLSimion-Lib/timer.h"

#include <algorithm>
int main()
{
	CMemManager<CSimionMemPool> *pMemManager= new CMemManager<CSimionMemPool>();
	size_t numElements = 10 * 1024 * 1024;
	CTimer timer;

	IMemBuffer* bufferHandler1= pMemManager->getMemBuffer(numElements);
	IMemBuffer* bufferHandler2 = pMemManager->getMemBuffer(numElements);
	IMemBuffer* bufferHandler3 = pMemManager->getMemBuffer(numElements);
	IMemBuffer* bufferHandler4 = pMemManager->getMemBuffer(numElements);
	IMemBuffer* bufferHandler5 = pMemManager->getMemBuffer(numElements);
	IMemBuffer* bufferHandler6 = pMemManager->getMemBuffer(numElements);
	IMemBuffer* bufferHandler7 = pMemManager->getMemBuffer(numElements);
	IMemBuffer* bufferHandler8 = pMemManager->getMemBuffer(numElements);
	IMemBuffer* bufferHandler9 = pMemManager->getMemBuffer(numElements);
	IMemBuffer* bufferHandler10 = pMemManager->getMemBuffer(numElements);

	pMemManager->init();
	timer.startTimer();
	size_t lastMemAlloc = pMemManager->getTotalAllocatedMem();
	int numAccesses = 10000000;
	int minIndex = 600000;
	int maxIndex = 5000000;
	double sum= 0.0;
	for (int i = 0; i < numElements; ++i)
	{
		double r = (double)rand() / (double)RAND_MAX;
		size_t index = (int) (r* (double)numElements);
		index = std::min(index, numElements - 1);// minIndex + r* (double)(maxIndex - minIndex);
		//int index = i;

		(*bufferHandler1)[index] = 1.0;
		(*bufferHandler2)[index] = 2.0;
		(*bufferHandler3)[index] = 3.0;
		(*bufferHandler4)[index] = 4.0;
		(*bufferHandler5)[index] = 5.0;
		(*bufferHandler6)[index] = 1.0;
		(*bufferHandler7)[index] = 2.0;
		(*bufferHandler8)[index] = 3.0;
		(*bufferHandler9)[index] = 4.0;
		(*bufferHandler10)[index] = 5.0;
		sum += (*bufferHandler1)[index] + (*bufferHandler2)[index]
			+ (*bufferHandler3)[index] + (*bufferHandler4)[index] + (*bufferHandler5)[index];
	}

	double time= timer.getElapsedTime();
	printf("Total memory allocated= %zd", pMemManager->getTotalAllocatedMem());
	printf("Total time= %f\n", time);
	printf("Sum= %f\n", sum);
	delete pMemManager;
	_CrtDumpMemoryLeaks();
    return 0;
}

