#include "stdafx.h"
#include "mem-pool.h"
#include "mem-buffer-handler.h"
#include "mem-block.h"

CSimpleMemPool::CSimpleMemPool(int elementCount) {}
CSimpleMemPool::~CSimpleMemPool()
{
	for (auto it = m_buffers.begin(); it != m_buffers.end(); ++it)
		delete *it;
}

IMemBuffer* CSimpleMemPool::getHandler(int elementCount)
{
	m_buffers.push_back(new CSimpleMemBufferHandler(this, elementCount));
	return m_buffers.back();
}

void CSimpleMemPool::init(int blockSize)
{
}

CInterleavedMemPool::CInterleavedMemPool(int numElements)
{
	m_numElements = numElements;
}


CInterleavedMemPool::~CInterleavedMemPool()
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


void CInterleavedMemPool::addMemBufferHandler(CInterLeavedMemBufferHandler* pMemBufferHandler)
{
	int newMemBufferOffset = m_elementSize;
	m_elementSize += pMemBufferHandler->getElementSize();
	m_memBufferHandlers.push_back(pMemBufferHandler);
}

IMemBuffer* CInterleavedMemPool::getHandler(int elementCount)
{
	CInterLeavedMemBufferHandler* pHandler = new CInterLeavedMemBufferHandler(this, 1, m_elementSize);
	addMemBufferHandler(pHandler);
	return pHandler;
}


double& CInterleavedMemPool::get(int elementIndex, int bufferOffset)
{
	int elementStartByte = elementIndex*m_elementSize + bufferOffset;
	int blockId = elementStartByte / m_memBlockSize;
	int relBlockAddr = elementStartByte % m_memBlockSize;

	CMemBlock* pBlock = m_memBlocks[blockId];

	return (*pBlock)[relBlockAddr];
}

void CInterleavedMemPool::init(int blockSize)
{
	CMemBlock* pNewMemBlock;
	m_memBlockSize = std::max(blockSize,512*1024);
	int totalMemSize = m_numElements * (int)m_memBufferHandlers.size();
	int numBlocks= totalMemSize / m_memBlockSize;
	for (int i = 0; i < numBlocks; ++i)
	{
		pNewMemBlock = new CMemBlock(this, m_memBlockSize);
		m_memBlocks.push_back(pNewMemBlock);
	}
}