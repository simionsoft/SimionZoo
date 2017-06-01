#pragma once
#include "mem-interfaces.h"
class CSimionMemPool;

class CSimpleMemBuffer : public IMemBuffer
{
	IMemPool* m_pPool;
	double* m_pBuffer = 0;
	int m_blockSize = 0;
public:
	CSimpleMemBuffer(IMemPool* pPool, int elementCount);
	~CSimpleMemBuffer();

	double& operator[](size_t index);
};

class CSimionMemBuffer: public IMemBuffer
{
	CSimionMemPool* m_pPool;
	int m_elementSize= 0, m_offset= 0;
public:
	//pParentPool: the pool this bufferhandler belongs to
	//elementCount: number of "double" elements in the buffer
	//offset: number of bytes from the beginning of an element from which this subelements starts
	CSimionMemBuffer(CSimionMemPool* pParentPool, int elementCount, int elementSize,int offset);
	virtual ~CSimionMemBuffer();

	//this is meant to be used in case types with different sizes are to be interleaved
	//as all the big buffers are, so far, double*, we skip this assume m_elementSize= 1
	int getElementSize() { return m_elementSize; }
	int getOffset() { return m_offset; }
	int getBlockSizeInBytes();
	CSimionMemPool* getPool() { return m_pPool; }

	double& operator[](size_t index);
};

