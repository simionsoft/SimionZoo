#include "stdafx.h"
#include "mem-buffer.h"
#include "mem-block.h"
#include "mem-pool.h"


CSimpleMemBuffer::CSimpleMemBuffer(IMemPool* pPool, int elementCount)
	:m_pPool(pPool), m_blockSize(elementCount),IMemBuffer(pPool,elementCount)
{
	m_pBuffer = new double[elementCount];
	pPool->updateTotalMemAllocated(elementCount * sizeof(double));
}
CSimpleMemBuffer::~CSimpleMemBuffer()
{
	if (m_pBuffer != nullptr) delete[] m_pBuffer;
}

double& CSimpleMemBuffer::operator[](size_t index)
{
	return m_pBuffer[index];
}




CSimionMemBuffer::CSimionMemBuffer(CSimionMemPool* pPool
	,int elementCount, int elementSize, int offset)
	:m_pPool(pPool), m_elementSize(elementSize), m_offset(offset),IMemBuffer(pPool,elementCount)
{

}

CSimionMemBuffer::~CSimionMemBuffer()
{
}

double& CSimionMemBuffer::operator[](size_t index)
{
	return m_pPool->get((int)index,m_offset);
}