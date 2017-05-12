#pragma once

#include "mem-block.h"
#include "mem-buffer-handler.h"
#include "mem-interfaces.h"
#include "mem-pool.h"
#include "delayed-load.h"

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
	CMemManager():CDeferredLoad(2){}
	virtual ~CMemManager()
	{
		for (auto it = m_memPools.begin(); it != m_memPools.end(); ++it)
		{
			delete *it;
		}
	}

	IMemBuffer* getMemBuffer(int elementCount)
	{
		IMemPool* pMemPool = getMemPool(elementCount);
		return pMemPool->getHandler(elementCount);
	}

	void init(int blockSize = 1024 * 1024)
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

	void deferredLoadStep()
	{
		for (auto it = m_memPools.begin(); it != m_memPools.end(); ++it)
			(*it)->init();
	}
};

