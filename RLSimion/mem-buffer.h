#pragma once

#include "mem-interfaces.h"
class SimionMemPool;

class SimpleMemBuffer : public IMemBuffer
{
	IMemPool* m_pPool;
	double* m_pBuffer = 0;
	BUFFER_SIZE m_blockSize = 0;
public:
	SimpleMemBuffer(IMemPool* pPool, BUFFER_SIZE elementCount);
	~SimpleMemBuffer();

	double& operator[](BUFFER_SIZE index);
};

class SimionMemBuffer: public IMemBuffer
{
	SimionMemPool* m_pPool;
	BUFFER_SIZE m_elementSize= 0, m_offset= 0;
public:
	//pParentPool: the pool this bufferhandler belongs to
	//elementCount: number of "double" elements in the buffer
	//offset: number of bytes from the beginning of an element from which this subelements starts
	SimionMemBuffer(SimionMemPool* pParentPool, BUFFER_SIZE elementCount
		, BUFFER_SIZE elementSize, BUFFER_SIZE offset);
	virtual ~SimionMemBuffer();

	//this is meant to be used in case types with different sizes are to be interleaved
	//as all the big buffers are, so far, double*, we skip this assume m_elementSize= 1
	BUFFER_SIZE getElementSize() { return m_elementSize; }
	BUFFER_SIZE getOffset() { return m_offset; }
	BUFFER_SIZE getBlockSizeInBytes();
	SimionMemPool* getPool() { return m_pPool; }

	double& operator[](BUFFER_SIZE index);
};

