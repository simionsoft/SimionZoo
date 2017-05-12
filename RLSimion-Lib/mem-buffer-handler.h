#pragma once
#include "mem-interfaces.h"
class CInterleavedMemPool;

class CSimpleMemBufferHandler : public IMemBuffer
{
	IMemPool* m_pPool;
	double* m_pBuffer = 0;
	int m_blockSizeInBytes = 0;
public:
	CSimpleMemBufferHandler(IMemPool* pPool, int elementCount);
	~CSimpleMemBufferHandler();

	double& operator[](size_t index);
};

class CInterLeavedMemBufferHandler: public IMemBuffer
{
	CInterleavedMemPool* m_pPool;
	int m_elementSize= 0, m_offset= 0;
public:
	//pParentPool: the pool this bufferhandler belongs to
	//elementSize: size of the elements we want to add to the interleaved memory buffer
	//             it will be 1 as long as we are only interested in using double data type
	//offset: number of bytes from the beginning of an element from which this subelements starts
	CInterLeavedMemBufferHandler(CInterleavedMemPool* pParentPool, int elementCount,int offset);
	virtual ~CInterLeavedMemBufferHandler();

	//this is meant to be used in case types with different sizes are to be interleaved
	//as all the big buffers are, so far, double*, we skip this assume m_elementSize= 1
	int getElementSize() { return m_elementSize; }
	int getOffset() { return m_offset; }
	CInterleavedMemPool* getPool() { return m_pPool; }

	double& operator[](size_t index);
};

