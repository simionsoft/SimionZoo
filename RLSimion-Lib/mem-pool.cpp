#include "stdafx.h"
#include "mem-pool.h"
#include "mem-buffer.h"
#include "mem-block.h"
#include "mem-manager.h"
#include <string>

CSimpleMemPool::CSimpleMemPool(int elementCount) {}
CSimpleMemPool::~CSimpleMemPool()
{
	for (auto it = m_buffers.begin(); it != m_buffers.end(); ++it)
		delete *it;
}

IMemBuffer* CSimpleMemPool::getHandler(int elementCount)
{
	m_buffers.push_back(new CSimpleMemBuffer(this, elementCount));
	return m_buffers.back();
}

void CSimpleMemPool::init(int blockSize)
{
}

void CSimpleMemPool::copy(IMemBuffer* pSrc, IMemBuffer* pDst)
{
	unsigned int numElements = pSrc->getNumElements();
	if ( numElements == pDst->getNumElements())
	{
		for (unsigned int i= 0; i<numElements; ++i)
		{
			(*pDst)[i] = (*pSrc)[i];
		}
	}
}



//Interleaved Memory Pool
//a set arrays with the same size are interleaved to improve cache hits

CSimionMemPool::CSimionMemPool(int numElements)
{
	m_numElements = numElements;
}


CSimionMemPool::~CSimionMemPool()
{
	for (auto it = m_memBufferHandlers.begin(); it != m_memBufferHandlers.end(); ++it)
	{
		delete *it;
	}
	for (auto it = m_memBlocks.begin(); it != m_memBlocks.end(); ++it)
	{
		delete *it;
	}
}


void CSimionMemPool::addMemBufferHandler(CSimionMemBuffer* pMemBufferHandler)
{
	int newMemBufferOffset = m_elementSize;
	m_elementSize += pMemBufferHandler->getElementSize();
	m_memBufferHandlers.push_back(pMemBufferHandler);
}

IMemBuffer* CSimionMemPool::getHandler(int elementCount)
{
	CSimionMemBuffer* pHandler 
		= new CSimionMemBuffer(this, elementCount, 1, m_elementSize);
	addMemBufferHandler(pHandler);
	return pHandler;
}


double& CSimionMemPool::get(int elementIndex, int bufferOffset)
{
	++m_accessCounter;

	int elementStartByte = elementIndex*m_elementSize + bufferOffset;
	int blockId = elementStartByte / m_memBlockSize;
	int relBlockAddr = elementStartByte % m_memBlockSize;
	double* pMemBuffer= 0;
	CMemBlock* pBlock = m_memBlocks[blockId];

	if (!pBlock->bAllocated())
	{
		//can we allocate more memory?
		int allocatedMem = getTotalAllocatedMem();
		int requestedMem = m_memBlockSize * sizeof(double);

		if (m_memLimit==0 || allocatedMem+requestedMem<=m_memLimit)
		{
			//try to allocate the memory buffer
			pMemBuffer = tryToAllocateMem(pBlock->size());

			if (pMemBuffer)
			{
				//memory block successfully allocated
				m_sortedAllocatedMemBlocks.push_front(pBlock);
				pBlock->setBuffer(pMemBuffer);
				m_totalAllocatedMem+= m_memBlockSize * sizeof(double);
			}
		}
		if (!pMemBuffer)
		{
			//failed to allocate the memory block: either permission was denied or there is no more available memory
			//recycle some already allocated memory block after dumping it to a file
			pBlock->setBuffer(recycleMem());
			m_sortedAllocatedMemBlocks.push_front(pBlock);
		}
		//initialization
		if (!pBlock->bInitialized())
			initialize(pBlock);
		else pBlock->restoreFromFile();
	}

	return (*pBlock)[relBlockAddr];
}

bool compare_lastAccess(CMemBlock* pFirst, CMemBlock* pSecond)
{
	return (pFirst->getLastAccess() >= pSecond->getLastAccess());
}

double* CSimionMemPool::recycleMem()
{
	m_sortedAllocatedMemBlocks.sort(compare_lastAccess);

	CMemBlock* pRecycledMemBlock = m_sortedAllocatedMemBlocks.front();
	pRecycledMemBlock->dumpToFile();
	double* pBuffer= pRecycledMemBlock->deallocate();
	m_sortedAllocatedMemBlocks.pop_front();
	
	return pBuffer;
}

void CSimionMemPool::initialize(CMemBlock* pBlock)
{
	int blockId = pBlock->getId();
	int firstElement = blockId*pBlock->size();
	int numHandlers = (int)m_memBufferHandlers.size();
	int handler;
	double initValue;
	for (int i = 0; i < pBlock->size(); ++i)
	{
		handler = (firstElement+i) % numHandlers;
		if (m_memBufferHandlers[handler]->bInitValueSet())
		{
			initValue = m_memBufferHandlers[handler]->getInitValue();
			(*pBlock)[i] = initValue;
		}
	}
	pBlock->setInitialized();
}

double* CSimionMemPool::tryToAllocateMem(int blockSize)
{
	double* pNewMemBlock;
	try
	{
		pNewMemBlock= new double[blockSize];
		return pNewMemBlock;
	}
	catch(std::exception ex)
	{
		return nullptr;
	}
}

void CSimionMemPool::init(int blockSize)
{
	CMemBlock* pNewMemBlock;
	int totalNumElements= m_numElements * (int)m_memBufferHandlers.size();
	m_memBlockSize = std::min(blockSize,totalNumElements);

	int numBlocks= totalNumElements / m_memBlockSize;
	if (totalNumElements % m_memBlockSize > 0)
		++numBlocks;

	for (int i = 0; i < numBlocks; ++i)
	{
		pNewMemBlock = new CMemBlock(this,i, m_memBlockSize);
		m_memBlocks.push_back(pNewMemBlock);
	}

	//we may have to correct the maximum amount of memory allowed to accomodate at least one block
	if (m_memLimit>0)
		m_memLimit = std::max(m_memLimit, (int)(m_memBlockSize * sizeof(double)));
}

void CSimionMemPool::copy(IMemBuffer* pSrc, IMemBuffer* pDst)
{
	CSimionMemBuffer* pSrcBuffer = dynamic_cast<CSimionMemBuffer*>(pSrc);
	CSimionMemBuffer* pDstBuffer = dynamic_cast<CSimionMemBuffer*>(pDst);

	if (pSrcBuffer && pDstBuffer)
	{
		//copy only those blocks that have been allocated and initialized
		for (int i = 0; i < pSrcBuffer->getNumElements(); ++i)
		{
			int absIndex = i*m_elementSize + pSrcBuffer->getOffset();
			int blockId= absIndex/m_memBlockSize;

			if (m_memBlocks[blockId]->bAllocated() && m_memBlocks[blockId]->bInitialized())
			{

				(*pDstBuffer)[i] = (*pSrcBuffer)[i];
			}
		}
	}
}