#pragma once

#include "mem-interfaces.h"
class CSimionMemPool;

class CSimpleMemBuffer : public IMemBuffer
{
	IMemPool* m_pPool;
	double* m_pBuffer = 0;
	BUFFER_SIZE m_blockSize = 0;
public:
	CSimpleMemBuffer(IMemPool* pPool, BUFFER_SIZE elementCount);
	~CSimpleMemBuffer();

	double& operator[](BUFFER_SIZE index);
};

class CSimionMemBuffer: public IMemBuffer
{
	CSimionMemPool* m_pPool;
	BUFFER_SIZE m_elementSize= 0, m_offset= 0;
public:
	//pParentPool: the pool this bufferhandler belongs to
	//elementCount: number of "double" elements in the buffer
	//offset: number of bytes from the beginning of an element from which this subelements starts
	CSimionMemBuffer(CSimionMemPool* pParentPool, BUFFER_SIZE elementCount
		, BUFFER_SIZE elementSize, BUFFER_SIZE offset);
	virtual ~CSimionMemBuffer();

	//this is meant to be used in case types with different sizes are to be interleaved
	//as all the big buffers are, so far, double*, we skip this assume m_elementSize= 1
	BUFFER_SIZE getElementSize() { return m_elementSize; }
	BUFFER_SIZE getOffset() { return m_offset; }
	BUFFER_SIZE getBlockSizeInBytes();
	CSimionMemPool* getPool() { return m_pPool; }

	double& operator[](BUFFER_SIZE index);
};

