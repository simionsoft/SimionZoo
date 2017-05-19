#pragma once

#include "mem-interfaces.h"
#include "mem-block.h"
#include "mem-buffer.h"
#include "mem-pool.h"
#include "deferred-load.h"

#include <vector>
using namespace std;

template <typename MemPoolType>
class CMemManager: public CDeferredLoad
{
	//Global collection of already requested memory pools
	//when a new memory buffer is requested, we search for an existing pool that uses the exact same
	//amount of elements. The goal is to interleave data and thus, reduce the number of cache errors
	//This should be a short list. Not likely worth using a map instead of a vector
	vector<IMemPool*>m_memPools;
	
	IMemPool* getMemPool(int elementCount)
	{
		for (auto it = m_memPools.begin(); it != m_memPools.end(); ++it)
		{
			if ((*it)->bCanAllocate(elementCount))
			{
				return (*it);
			}
		}

		m_memPools.push_back(new MemPoolType(elementCount));
		return m_memPools.back();
	}
public:
	//This should be the first deferred load step taken
	CMemManager():CDeferredLoad(10){}
	virtual ~CMemManager()
	{
		for (auto it = m_memPools.begin(); it != m_memPools.end(); ++it)
		{
			delete *it;
		}
	}

	//maxAllocatedMemory: maximum number of bytes allowed to have in memory concurrently
	void setMaxAllocatedMem(int maxAllocatedMem)
	{
		for (auto it = m_memPools.begin(); it != m_memPools.end(); ++it)
		{
			(*it)->setMemLimit(maxAllocatedMem);
		}
	}

	bool bAskPermissionAllocateMemBuffer(int memSizeRequested)
	{
		if (m_maxAllocatedMem < 0) return true;
		int allocatedMem = getAllocatedMem();

		if (allocatedMem + memSizeRequested > m_maxAllocatedMem)
			return false;
		return true;
	}

	IMemBuffer* getMemBuffer(int elementCount)
	{
		IMemPool* pMemPool = getMemPool(elementCount);
		return pMemPool->getHandler(elementCount);
	}

	void init(int blockSize = 64 * 1024)
	{
		for (auto it = m_memPools.begin(); it != m_memPools.end(); ++it)
			(*it)->init(blockSize);
	}

	int getTotalAllocatedMem() const
	{
		int total = 0;
		for (auto it = m_memPools.begin(); it != m_memPools.end(); ++it)
		{
			total += (*it)->getTotalAllocatedMem();
		}
		return total;
	}

	//This method copies initialized memory blocks from buffer to buffer
	//Don't use in performance-critical operations done frequently such as copying weights
	//from a function to the frozen copy. Rather, store in a separate feature list
	//weight updates and then apply those updates to the frozen copy.
	virtual void copy(IMemBuffer* pSrc, IMemBuffer* pDst)
	{
		IMemPool* pSrcPool= pSrc->getMemPool();
		IMemPool* pDstPool = pDst->getMemPool();
		if (pSrcPool == pDstPool)
		{
			pSrcPool->copy(pSrc, pDst);
		}
	}

	void deferredLoadStep()
	{
		init();
	}
};

