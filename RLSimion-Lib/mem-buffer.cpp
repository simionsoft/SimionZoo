#include "stdafx.h"
#include "mem-pool.h"
#include "mem-block.h"
#include "mem-buffer.h"


CSimpleMemBuffer::CSimpleMemBuffer(IMemPool* pPool, BUFFER_SIZE elementCount)
	:m_pPool(pPool), m_blockSize(elementCount),IMemBuffer(pPool,elementCount)
{
	m_pBuffer = new double[elementCount];
	pPool->updateTotalMemAllocated(elementCount * sizeof(double));
}
CSimpleMemBuffer::~CSimpleMemBuffer()
{
	if (m_pBuffer != nullptr) delete[] m_pBuffer;
}

double& CSimpleMemBuffer::operator[](BUFFER_SIZE index)
{
	return m_pBuffer[index];
}




CSimionMemBuffer::CSimionMemBuffer(CSimionMemPool* pPool
	, BUFFER_SIZE elementCount, BUFFER_SIZE elementSize, BUFFER_SIZE offset)
	:m_pPool(pPool), m_elementSize(elementSize), m_offset(offset),IMemBuffer(pPool,elementCount)
{

}

CSimionMemBuffer::~CSimionMemBuffer()
{
}

double& CSimionMemBuffer::operator[](BUFFER_SIZE index)
{
	return m_pPool->get((int)index,m_offset);
}

BUFFER_SIZE CSimionMemBuffer::getBlockSizeInBytes()
{
	return m_pPool->getBlockSize()*sizeof(double);
}