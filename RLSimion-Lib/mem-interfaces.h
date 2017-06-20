#pragma once
#include "mem-manager.h"
class IMemBuffer;

class IMemPool
{
protected:
	BUFFER_SIZE m_totalAllocatedMem = 0;
	BUFFER_SIZE m_memLimit = 0;
public:
	virtual ~IMemPool() {};

	virtual IMemBuffer* getHandler(BUFFER_SIZE elementCount)= 0;
	virtual void init(BUFFER_SIZE blockSize= 524288) = 0;
	virtual bool bCanAllocate(BUFFER_SIZE elementCount) const = 0;
	virtual void copy(IMemBuffer* pSrc, IMemBuffer* pDst) = 0;

	virtual void setMemLimit(BUFFER_SIZE memLimit) { m_memLimit = memLimit; }

	BUFFER_SIZE getTotalAllocatedMem() const { return m_totalAllocatedMem; }
	void updateTotalMemAllocated(BUFFER_SIZE inc) { m_totalAllocatedMem += inc; }
};

class IMemBuffer
{
	IMemPool* m_pPool;
	double m_initValue;
	bool m_bInitValueSet = false;

	BUFFER_SIZE m_numElements;
public:
	IMemBuffer(IMemPool* pPool, BUFFER_SIZE numElements) { m_pPool = pPool; m_numElements = numElements; }
	virtual ~IMemBuffer() {};

	virtual double& operator[](BUFFER_SIZE index)= 0;
	void setInitValue(double value) { m_initValue = value; m_bInitValueSet = true; }
	bool bInitValueSet() const { return m_bInitValueSet; }
	double getInitValue() const { return m_initValue; }
	BUFFER_SIZE getNumElements() const { return m_numElements; }
	IMemPool* getMemPool() const { return m_pPool; }
};
