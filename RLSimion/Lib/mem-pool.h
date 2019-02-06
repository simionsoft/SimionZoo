#pragma once

#include "mem-manager.h"

#include <vector>
#include <list>
using namespace std;

class MemBlock;

class SimpleMemPool : public IMemPool
{
	vector<IMemBuffer*> m_buffers;

public:
	SimpleMemPool(BUFFER_SIZE elementCount);
	virtual ~SimpleMemPool();
	virtual IMemBuffer* getHandler(BUFFER_SIZE elementCount);
	virtual bool bCanAllocate(BUFFER_SIZE elementCount) const { return true; }

	void copy(IMemBuffer* pSrc, IMemBuffer* pDst);

	virtual void init(BUFFER_SIZE blockSize);
};

class SimionMemPool: public IMemPool
{
	friend class SimionMemBuffer;
	friend class MemBlock;
	
	//Local collection of mem. buffer handlers
	vector<SimionMemBuffer*> m_memBufferHandlers;
	vector<MemBlock*> m_memBlocks;
	vector<MemBlock*> m_allocatedMemBlocks;

	void addMemBufferHandler(SimionMemBuffer* pMemBufferHandler);
	//This function returns a buffer of size elementCount*sizeof(double)
	//or nullptr if "bad_allocation" exception was raised
	double* tryToAllocateMem(BUFFER_SIZE elementCount);
	//This function dumpls to a file the memory allocated to some other memory block
	//marks it as "not allocated" and returns the buffer for recycling
	double* recycleMem();
	void initialize(MemBlock* pBlock);

	double& get(BUFFER_SIZE elementIndex, BUFFER_SIZE bufferOffset);

	BUFFER_SIZE m_elementSize = 0;
	BUFFER_SIZE m_numElements = 0;
	BUFFER_SIZE m_memBlockSize = 0;
	BUFFER_SIZE m_accessCounter = 0;
public:
	SimionMemPool(BUFFER_SIZE elementCount);
	virtual ~SimionMemPool();

	virtual BUFFER_SIZE getNumElements() const { return m_numElements; }
	BUFFER_SIZE getElementSize() const { return m_elementSize; }
	BUFFER_SIZE getBlockSize() const { return m_memBlockSize; }
	virtual bool bCanAllocate(BUFFER_SIZE elementCount) const { return elementCount == m_numElements; }
	BUFFER_SIZE getAccessCounter();
	void resetAccessCounter();

	virtual IMemBuffer* getHandler(BUFFER_SIZE elementCount);
	void copy(IMemBuffer* pSrc, IMemBuffer* pDst);

	//This method must be called after all the SimionMemBuffer's are requested
	void init(BUFFER_SIZE blockSize);
};

