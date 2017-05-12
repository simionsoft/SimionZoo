#pragma once
#include "mem-interfaces.h"

class CInterLeavedMemBufferHandler;
class CMemBlock;
#include <vector>
using namespace std;

class CSimpleMemPool : public IMemPool
{
	vector<IMemBuffer*> m_buffers;
public:
	CSimpleMemPool(int elementCount);
	virtual ~CSimpleMemPool();
	virtual IMemBuffer* getHandler(int elementCount);
	virtual bool bCanAllocate(int elementCount) const { return true; }

	virtual void init(int blockSize);
};

class CInterleavedMemPool: public IMemPool
{
	friend class CInterLeavedMemBufferHandler;
	friend class CMemBlock;
	
	//Local collection of mem. buffer handlers
	vector<CInterLeavedMemBufferHandler*> m_memBufferHandlers;
	vector<CMemBlock*> m_memBlocks;

	void addMemBufferHandler(CInterLeavedMemBufferHandler* pMemBufferHandler);

	double& get(int elementIndex, int bufferOffset);

	int m_elementSize = 0;
	int m_numElements = 0;
	int m_memBlockSize = 0;
public:
	CInterleavedMemPool(int elementCount);
	virtual ~CInterleavedMemPool();

	virtual int getNumElements() const { return m_numElements; }
	int getElementSize() const { return m_elementSize; }
	int getBlockSize() const { return m_memBlockSize; }
	virtual bool bCanAllocate(int elementCount) const { return elementCount == m_numElements; }

	virtual IMemBuffer* getHandler(int elementCount);

	//This method must be called after all the CInterLeavedMemBufferHandler's are requested
	void init(int blockSize= 65536);
};

