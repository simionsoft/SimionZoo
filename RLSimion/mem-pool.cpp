#include "mem-pool.h"
#include "mem-buffer.h"
#include "mem-block.h"
#include "mem-manager.h"
#include <string>
#include <algorithm>

SimpleMemPool::SimpleMemPool(BUFFER_SIZE elementCount) {}
SimpleMemPool::~SimpleMemPool()
{
	for (auto it = m_buffers.begin(); it != m_buffers.end(); ++it)
		delete *it;
}

IMemBuffer* SimpleMemPool::getHandler(BUFFER_SIZE elementCount)
{
	m_buffers.push_back(new SimpleMemBuffer(this, elementCount));
	return m_buffers.back();
}

void SimpleMemPool::init(BUFFER_SIZE blockSize)
{
}

void SimpleMemPool::copy(IMemBuffer* pSrc, IMemBuffer* pDst)
{
	BUFFER_SIZE numElements = pSrc->getNumElements();
	if ( numElements == pDst->getNumElements())
	{
		for (BUFFER_SIZE i= 0; i<numElements; ++i)
		{
			(*pDst)[i] = (*pSrc)[i];
		}
	}
}



//Interleaved Memory Pool
//a set arrays with the same size are interleaved to improve cache hits

SimionMemPool::SimionMemPool(BUFFER_SIZE numElements)
{
	m_numElements = numElements;
}


SimionMemPool::~SimionMemPool()
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


void SimionMemPool::addMemBufferHandler(SimionMemBuffer* pMemBufferHandler)
{
	m_elementSize += pMemBufferHandler->getElementSize();
	m_memBufferHandlers.push_back(pMemBufferHandler);
}

IMemBuffer* SimionMemPool::getHandler(BUFFER_SIZE elementCount)
{
	SimionMemBuffer* pHandler 
		= new SimionMemBuffer(this, elementCount, 1, m_elementSize);
	addMemBufferHandler(pHandler);
	return pHandler;
}


double& SimionMemPool::get(BUFFER_SIZE elementIndex, BUFFER_SIZE bufferOffset)
{
	++m_accessCounter;

	BUFFER_SIZE elementStartByte = elementIndex*m_elementSize + bufferOffset;
	BUFFER_SIZE blockId = elementStartByte / m_memBlockSize;
	BUFFER_SIZE relBlockAddr = elementStartByte % m_memBlockSize;
	double* pMemBuffer= 0;
	MemBlock* pBlock = m_memBlocks[(size_t)blockId];

	if (!pBlock->bAllocated())
	{
		//can we allocate more memory?
		BUFFER_SIZE allocatedMem = getTotalAllocatedMem();
		BUFFER_SIZE requestedMem = m_memBlockSize * sizeof(double);

		if (m_memLimit == 0 || allocatedMem + requestedMem <= m_memLimit)
		{
			//try to allocate the memory buffer
			pMemBuffer = tryToAllocateMem(pBlock->size());

			if (pMemBuffer)
			{
				//memory block successfully allocated
				m_allocatedMemBlocks.push_back(pBlock);
				pBlock->setBuffer(pMemBuffer);
				m_totalAllocatedMem += m_memBlockSize * sizeof(double);
			}
		}
		if (!pMemBuffer)
		{
			//failed to allocate the memory block: either permission was denied or there is no more available memory
			//recycle some already allocated memory block after dumping it to a file
			pBlock->setBuffer(recycleMem());
			m_allocatedMemBlocks.push_back(pBlock);
		}
		//initialization
		if (!pBlock->bInitialized())
			initialize(pBlock);
		else pBlock->restoreFromFile();
	}


	return (*pBlock)[relBlockAddr];
}

bool compare_lastAccess(MemBlock* pFirst, MemBlock* pSecond)
{
	return (pFirst->getLastAccess() > pSecond->getLastAccess());
}

double* SimionMemPool::recycleMem()
{
	std::sort(m_allocatedMemBlocks.begin(),m_allocatedMemBlocks.end(),compare_lastAccess);

	//blocks are sorted from last accest to oldest accest
	MemBlock* pRecycledMemBlock = m_allocatedMemBlocks.back();
	pRecycledMemBlock->dumpToFile();
	double* pBuffer= pRecycledMemBlock->deallocate();
	m_allocatedMemBlocks.pop_back();
	
	return pBuffer;
}

void SimionMemPool::resetAccessCounter()
{
	std::sort(m_allocatedMemBlocks.begin(), m_allocatedMemBlocks.end(), compare_lastAccess);

	BUFFER_SIZE oldestAccessCounter = m_allocatedMemBlocks.back()->getLastAccess();
	for (auto it = m_allocatedMemBlocks.begin(); it != m_allocatedMemBlocks.end(); ++it)
		(*it)->setLastAccess((*it)->getLastAccess() - oldestAccessCounter);

	m_accessCounter = 0;
}

void SimionMemPool::initialize(MemBlock* pBlock)
{
	BUFFER_SIZE blockId = pBlock->getId();
	size_t firstElement = blockId*pBlock->size();
	size_t numHandlers = (int)m_memBufferHandlers.size();
	size_t handler;
	double initValue;
	for (int i = 0; i < (int) pBlock->size(); ++i)
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

double* SimionMemPool::tryToAllocateMem(BUFFER_SIZE blockSize)
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

void SimionMemPool::init(BUFFER_SIZE blockSize)
{
	MemBlock* pNewMemBlock;
	size_t totalNumElements= m_numElements * (int)m_memBufferHandlers.size();
	m_memBlockSize = std::min(blockSize,totalNumElements);

	//make the block size a multiple of the number of interleaved arrays
	m_memBlockSize -= m_memBlockSize % m_elementSize;

	size_t numBlocks= totalNumElements / m_memBlockSize;
	if (totalNumElements % m_memBlockSize > 0)
		++numBlocks;

	//pre-allocate space in both vectors
	m_memBlocks.reserve(numBlocks);
	m_allocatedMemBlocks.reserve(numBlocks);

	for (size_t i = 0; i < numBlocks; ++i)
	{
		pNewMemBlock = new MemBlock(this,i, m_memBlockSize);
		m_memBlocks.push_back(pNewMemBlock);
	}

	//we may have to correct the maximum amount of memory allowed to accomodate at least one block
	if (m_memLimit>0)
		m_memLimit = std::max(m_memLimit, (BUFFER_SIZE)(m_memBlockSize * sizeof(double)));
}

void SimionMemPool::copy(IMemBuffer* pSrc, IMemBuffer* pDst)
{
	SimionMemBuffer* pSrcBuffer = dynamic_cast<SimionMemBuffer*>(pSrc);
	SimionMemBuffer* pDstBuffer = dynamic_cast<SimionMemBuffer*>(pDst);
	size_t numBlocksCopied = 0;
	if (pSrcBuffer && pDstBuffer)
	{
		//copy only those blocks that have been allocated and initialized
		size_t minRelIndexInBlock, maxRelIndexInBlock;
		size_t blockAbsOffset = 0;
		minRelIndexInBlock = blockAbsOffset / m_elementSize;
		if (pSrcBuffer->getOffset()<(blockAbsOffset%m_elementSize))
			++minRelIndexInBlock;
		for (size_t block = 0; block < m_memBlocks.size(); ++block)
		{
			if (m_memBlocks[block]->bAllocated() && m_memBlocks[block]->bInitialized())
			{
				maxRelIndexInBlock = (blockAbsOffset+m_memBlockSize)/m_elementSize;
				if (pSrcBuffer->getOffset()<((blockAbsOffset+m_memBlockSize)%m_elementSize))
						++maxRelIndexInBlock;

				for (size_t i= minRelIndexInBlock; i<maxRelIndexInBlock; ++i)
					(*pDstBuffer)[i] = (*pSrcBuffer)[i];
				++numBlocksCopied;
			}
			blockAbsOffset += m_memBlockSize;
			minRelIndexInBlock = maxRelIndexInBlock;
		}
	}
}

BUFFER_SIZE SimionMemPool::getAccessCounter()
{
	return ++m_accessCounter;
}