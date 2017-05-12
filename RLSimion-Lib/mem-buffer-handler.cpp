#include "stdafx.h"
#include "mem-buffer-handler.h"
#include "mem-block.h"
#include "mem-pool.h"


CSimpleMemBufferHandler::CSimpleMemBufferHandler(IMemPool* pPool, int elementCount)
	:m_pPool(pPool), m_blockSizeInBytes(elementCount)
{
	m_pBuffer = new double[elementCount];
	pPool->updateTotalMemAllocated(elementCount * sizeof(double));
}
CSimpleMemBufferHandler::~CSimpleMemBufferHandler()
{
	if (m_pBuffer != nullptr) delete[] m_pBuffer;
}

double& CSimpleMemBufferHandler::operator[](size_t index)
{
	return m_pBuffer[index];
}




CInterLeavedMemBufferHandler::CInterLeavedMemBufferHandler(CInterleavedMemPool* pParentPool, int subElementSize, int offset)
	:m_pPool (pParentPool), m_elementSize(subElementSize), m_offset(offset)
{

}

CInterLeavedMemBufferHandler::~CInterLeavedMemBufferHandler()
{
}

double& CInterLeavedMemBufferHandler::operator[](size_t index)
{
	return m_pPool->get((int)index,m_offset);
}