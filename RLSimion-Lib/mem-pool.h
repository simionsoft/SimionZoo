#pragma once
#include "mem-manager.h"
class CSimionMemPool;
#include <vector>
#include <list>
using namespace std;

class CSimpleMemPool : public IMemPool
{
	vector<IMemBuffer*> m_buffers;

public:
	CSimpleMemPool(int elementCount);
	virtual ~CSimpleMemPool();
	virtual IMemBuffer* getHandler(int elementCount);
	virtual bool bCanAllocate(int elementCount) const { return true; }

	void copy(IMemBuffer* pSrc, IMemBuffer* pDst);

	virtual void init(int blockSize);
};

class CSimionMemPool: public IMemPool
{
	friend class CSimionMemBuffer;
	friend class CMemBlock;
	
	//Local collection of mem. buffer handlers
	vector<CSimionMemBuffer*> m_memBufferHandlers;
	vector<CMemBlock*> m_memBlocks;
	list<CMemBlock*> m_sortedAllocatedMemBlocks;

	void addMemBufferHandler(CSimionMemBuffer* pMemBufferHandler);
	//This function returns a buffer of size elementCount*sizeof(double)
	//or nullptr if "bad_allocation" exception was raised
	double* tryToAllocateMem(int elementCount);
	//This function dumpls to a file the memory allocated to some other memory block
	//marks it as "not allocated" and returns the buffer for recycling
	double* recycleMem();
	void initialize(CMemBlock* pBlock);

	double& get(int elementIndex, int bufferOffset);

	int m_elementSize = 0;
	int m_numElements = 0;
	int m_memBlockSize = 0;
	int m_accessCounter = 0;
public:
	CSimionMemPool(int elementCount);
	virtual ~CSimionMemPool();

	virtual int getNumElements() const { return m_numElements; }
	int getElementSize() const { return m_elementSize; }
	int getBlockSize() const { return m_memBlockSize; }
	virtual bool bCanAllocate(int elementCount) const { return elementCount == m_numElements; }
	int getAccessCounter() const { return m_accessCounter; }

	virtual IMemBuffer* getHandler(int elementCount);
	void copy(IMemBuffer* pSrc, IMemBuffer* pDst);

	//This method must be called after all the CSimionMemBuffer's are requested
	void init(int blockSize);
};

