#include "stdafx.h"
#include "mem-block.h"
#include "mem-pool.h"

CMemBlock::CMemBlock(CInterleavedMemPool* pPool, int blockSize)
	: m_pPool(pPool), m_blockSizeInBytes(blockSize)
{
	//m_pBuffer = new double[m_blockSizeInBytes];
	//m_pPool->updateTotalMemAllocated(m_blockSizeInBytes);
}

CMemBlock::~CMemBlock()
{
	if (m_pBuffer != nullptr)
		delete[] m_pBuffer;
}

double& CMemBlock::operator[](size_t index)
{
	if (m_pBuffer != nullptr)
		return m_pBuffer[index];

	m_pBuffer = new double[m_blockSizeInBytes];
	m_pPool->updateTotalMemAllocated(m_blockSizeInBytes);
	return m_pBuffer[index];
}
