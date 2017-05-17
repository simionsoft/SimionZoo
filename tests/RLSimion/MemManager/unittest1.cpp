#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../../RLSimion-Lib/mem-manager.h"
#define BUFFER_SIZE 5*1024*1024
#define BLOCK_SIZE 512 * 1024
#define BLOCK_SIZE_IN_BYTES (BLOCK_SIZE*8)

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MemManager
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(MemManager_TotalAllocated)
		{
			// TODO: Your test code here
			CMemManager<CSimionMemPool>* pMemManager = new CMemManager<CSimionMemPool>();
			IMemBuffer* pBuffer1 = pMemManager->getMemBuffer(BUFFER_SIZE);
			IMemBuffer* pBuffer2 = pMemManager->getMemBuffer(BUFFER_SIZE);
			IMemBuffer* pBuffer3 = pMemManager->getMemBuffer(BUFFER_SIZE);
			IMemBuffer* pBuffer4 = pMemManager->getMemBuffer(BUFFER_SIZE);
			IMemBuffer* pBuffer5 = pMemManager->getMemBuffer(BUFFER_SIZE);
			
			pMemManager->init(BLOCK_SIZE);
			int realBlockSize = dynamic_cast<CSimionMemBuffer*>(pBuffer1)->getBlockSizeInBytes();

			Assert::AreEqual(pMemManager->getTotalAllocatedMem(), 0);

			(*pBuffer1)[0] = 1.0;
			(*pBuffer2)[0] = 2.0;
			(*pBuffer3)[0] = 3.0;
			(*pBuffer1)[1] = 1.0;
			(*pBuffer2)[1] = 2.0;
			(*pBuffer3)[1] = 3.0;
			Assert::AreEqual((*pBuffer1)[0], 1.0);
			Assert::AreEqual((*pBuffer2)[0], 2.0);
			Assert::AreEqual((*pBuffer3)[0], 3.0);
			Assert::AreEqual((*pBuffer1)[1], 1.0);
			Assert::AreEqual((*pBuffer2)[1], 2.0);
			Assert::AreEqual((*pBuffer3)[1], 3.0);

			Assert::AreEqual(realBlockSize,pMemManager->getTotalAllocatedMem());

			(*pBuffer1)[0 + realBlockSize] = -1.0;
			(*pBuffer2)[0 + realBlockSize] = -2.0;
			(*pBuffer3)[0 + realBlockSize] = -3.0;
			(*pBuffer1)[1 + realBlockSize] = -1.0;
			(*pBuffer2)[1 + realBlockSize] = -2.0;
			(*pBuffer3)[1 + realBlockSize] = -3.0;

			Assert::AreEqual((*pBuffer1)[0 + realBlockSize] ,-1.0);

			Assert::AreEqual(realBlockSize * 2, pMemManager->getTotalAllocatedMem());

			Assert::AreEqual((*pBuffer2)[0 + realBlockSize], -2.0);
			Assert::AreEqual((*pBuffer3)[0 + realBlockSize], -3.0);
			Assert::AreEqual((*pBuffer1)[1 + realBlockSize], -1.0);
			Assert::AreEqual((*pBuffer2)[1 + realBlockSize], -2.0);
			Assert::AreEqual((*pBuffer3)[1 + realBlockSize], -3.0);

			Assert::AreEqual(realBlockSize * 2, pMemManager->getTotalAllocatedMem());

			delete pMemManager;
		}
		TEST_METHOD(MemManager_InitValue)
		{
			// TODO: Your test code here
			CMemManager<CSimionMemPool>* pMemManager = new CMemManager<CSimionMemPool>();
			IMemBuffer* pBuffer1 = pMemManager->getMemBuffer(BUFFER_SIZE);
			pBuffer1->setInitValue(1.0);
			IMemBuffer* pBuffer2 = pMemManager->getMemBuffer(BUFFER_SIZE);
			pBuffer2->setInitValue(2.0);
			IMemBuffer* pBuffer3 = pMemManager->getMemBuffer(BUFFER_SIZE);
			pBuffer3->setInitValue(3.0);
			IMemBuffer* pBuffer4 = pMemManager->getMemBuffer(BUFFER_SIZE);
			pBuffer4->setInitValue(4.0);
			IMemBuffer* pBuffer5 = pMemManager->getMemBuffer(BUFFER_SIZE);
			pBuffer5->setInitValue(5.0);

			pMemManager->init(BLOCK_SIZE);

			Assert::AreEqual(1.0, (*pBuffer1)[0]);
			Assert::AreEqual(2.0, (*pBuffer2)[1000]);
			Assert::AreEqual(3.0, (*pBuffer3)[20]);
			Assert::AreEqual(4.0, (*pBuffer4)[210390]);
			Assert::AreEqual(5.0, (*pBuffer5)[12310]);
			
			delete pMemManager;
		}

		//Check that buffer copies work properly
#define SMALL_BUFER_SIZE 1024
#define MAX_NUM_ELEMENTS 3000
#define MAX_MEMORY (MAX_NUM_ELEMENTS*sizeof(double))
#define SMALL_BLOCK_SIZE 100
		TEST_METHOD(MemManager_Copy)
		{
			CMemManager<CSimionMemPool>* pMemManager = new CMemManager<CSimionMemPool>();
			IMemBuffer* pBuffer1 = pMemManager->getMemBuffer(SMALL_BUFER_SIZE);
			pBuffer1->setInitValue(1.0);
			IMemBuffer* pBuffer2 = pMemManager->getMemBuffer(SMALL_BUFER_SIZE);
			pBuffer2->setInitValue(2.0);
			IMemBuffer* pBuffer3 = pMemManager->getMemBuffer(SMALL_BUFER_SIZE);
			pBuffer3->setInitValue(3.0);
			IMemBuffer* pBuffer4 = pMemManager->getMemBuffer(SMALL_BUFER_SIZE);
			pBuffer4->setInitValue(4.0);
			IMemBuffer* pBuffer5 = pMemManager->getMemBuffer(SMALL_BUFER_SIZE);
			pBuffer5->setInitValue(5.0);

			pMemManager->init(SMALL_BLOCK_SIZE);

			Assert::AreEqual(1.0, (*pBuffer1)[0]);
			Assert::AreEqual(2.0, (*pBuffer2)[1000]);

			pMemManager->copy(pBuffer1, pBuffer2);

			Assert::AreEqual(1.0, (*pBuffer1)[0]);
			Assert::AreEqual(1.0, (*pBuffer2)[1000]);

			pMemManager->copy(pBuffer2, pBuffer3);

			Assert::AreEqual(1.0, (*pBuffer1)[0]);
			Assert::AreEqual(1.0, (*pBuffer2)[1000]);
			Assert::AreEqual(3.0, (*pBuffer3)[350]);

			pMemManager->copy(pBuffer4, pBuffer1);

			Assert::AreEqual(4.0, (*pBuffer1)[0]);
			Assert::AreEqual(1.0, (*pBuffer2)[1000]);
			Assert::AreEqual(3.0, (*pBuffer3)[350]);
			Assert::AreEqual(4.0, (*pBuffer4)[10]);

			Assert::IsTrue(pMemManager->getTotalAllocatedMem()< SMALL_BUFER_SIZE*sizeof(double)*4);

			delete pMemManager;
		}
		TEST_METHOD(MemManager_UpperBound)
		{
			CMemManager<CSimionMemPool>* pMemManager = new CMemManager<CSimionMemPool>();
			IMemBuffer* pBuffer1 = pMemManager->getMemBuffer(BUFFER_SIZE);
			pBuffer1->setInitValue(1.0);
			IMemBuffer* pBuffer2 = pMemManager->getMemBuffer(BUFFER_SIZE);
			pBuffer2->setInitValue(2.0);
			IMemBuffer* pBuffer3 = pMemManager->getMemBuffer(BUFFER_SIZE);
			pBuffer3->setInitValue(3.0);
			IMemBuffer* pBuffer4 = pMemManager->getMemBuffer(BUFFER_SIZE);
			pBuffer4->setInitValue(4.0);
			IMemBuffer* pBuffer5 = pMemManager->getMemBuffer(BUFFER_SIZE);
			pBuffer5->setInitValue(5.0);
			pMemManager->init();

			Assert::AreEqual(1.0, (*pBuffer1)[BUFFER_SIZE - 1]);
			Assert::AreEqual(2.0, (*pBuffer2)[BUFFER_SIZE - 1]);
			Assert::AreEqual(3.0, (*pBuffer3)[BUFFER_SIZE - 1]);
			Assert::AreEqual(4.0, (*pBuffer4)[BUFFER_SIZE - 1]);
			Assert::AreEqual(5.0, (*pBuffer5)[BUFFER_SIZE - 1]);

			delete pMemManager;
		}

		////////////////////////////////////////////////
		//Mem limit checks
		///////////////////////////////////////////////


		TEST_METHOD(MemManager_MemLimit)
		{
			CMemManager<CSimionMemPool>* pMemManager = new CMemManager<CSimionMemPool>();
			IMemBuffer* pBuffer1 = pMemManager->getMemBuffer(SMALL_BUFER_SIZE);
			pBuffer1->setInitValue(1.0);
			IMemBuffer* pBuffer2 = pMemManager->getMemBuffer(SMALL_BUFER_SIZE);
			pBuffer2->setInitValue(2.0);

			pMemManager->setMaxAllocatedMem(MAX_MEMORY);
			pMemManager->init(1024*1024);

			for (int i = 0; i < SMALL_BUFER_SIZE; ++i)
			{
				(*pBuffer1)[i]= i;
				(*pBuffer2)[i]= i*2;
			}
			Assert::IsTrue(MAX_MEMORY>pMemManager->getTotalAllocatedMem());

			delete pMemManager;
		}
		TEST_METHOD(MemManager_MemDiskDump)
		{
			CMemManager<CSimionMemPool>* pMemManager = new CMemManager<CSimionMemPool>();
			IMemBuffer* pBuffer1 = pMemManager->getMemBuffer(SMALL_BUFER_SIZE);
			pBuffer1->setInitValue(1.0);
			IMemBuffer* pBuffer2 = pMemManager->getMemBuffer(SMALL_BUFER_SIZE);
			pBuffer2->setInitValue(2.0);

			pMemManager->setMaxAllocatedMem(MAX_MEMORY);
			pMemManager->init(1024 * 1024);

			for (int i = 0; i < SMALL_BUFER_SIZE; ++i)
			{
				(*pBuffer1)[i] = i;
				(*pBuffer2)[i] = i * 2;
			}
			for (int i = 0; i < SMALL_BUFER_SIZE; ++i)
			{
				Assert::AreEqual((double)i,(*pBuffer1)[i]);
				Assert::AreEqual((double)i*2,(*pBuffer2)[i]);
			}
			Assert::IsTrue(MAX_MEMORY>pMemManager->getTotalAllocatedMem());

			delete pMemManager;
		}
	};
}