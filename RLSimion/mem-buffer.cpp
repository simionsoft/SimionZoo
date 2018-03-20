#include "mem-pool.h"
#include "mem-block.h"
#include "mem-buffer.h"


SimpleMemBuffer::SimpleMemBuffer(IMemPool* pPool, BUFFER_SIZE elementCount)
	:m_pPool(pPool), m_blockSize(elementCount),IMemBuffer(pPool,elementCount)
{
	m_pBuffer = new double[elementCount];
	pPool->updateTotalMemAllocated(elementCount * sizeof(double));
}
SimpleMemBuffer::~SimpleMemBuffer()
{
	if (m_pBuffer != nullptr) delete[] m_pBuffer;
}

double& SimpleMemBuffer::operator[](BUFFER_SIZE index)
{
	return m_pBuffer[index];
}




SimionMemBuffer::SimionMemBuffer(SimionMemPool* pPool
	, BUFFER_SIZE elementCount, BUFFER_SIZE elementSize, BUFFER_SIZE offset)
	:m_pPool(pPool), m_elementSize(elementSize), m_offset(offset),IMemBuffer(pPool,elementCount)
{

}

SimionMemBuffer::~SimionMemBuffer()
{
}

double& SimionMemBuffer::operator[](BUFFER_SIZE index)
{
	return m_pPool->get((int)index,m_offset);
}

BUFFER_SIZE SimionMemBuffer::getBlockSizeInBytes()
{
	return m_pPool->getBlockSize()*sizeof(double);
}