#pragma once

class IMemBuffer;

class IMemPool
{
protected:
	int m_totalAllocatedMem = 0;
	int m_memLimit = 0;
public:
	virtual ~IMemPool() {};

	virtual IMemBuffer* getHandler(int elementCount)= 0;
	virtual void init(int blockSize= 524288) = 0;
	virtual bool bCanAllocate(int elementCount) const = 0;
	virtual void copy(IMemBuffer* pSrc, IMemBuffer* pDst) = 0;

	virtual void setMemLimit(int memLimit) { m_memLimit = memLimit; }

	int getTotalAllocatedMem() const { return m_totalAllocatedMem; }
	void updateTotalMemAllocated(int inc) { m_totalAllocatedMem += inc; }
};

class IMemBuffer
{
	IMemPool* m_pPool;
	double m_initValue;
	bool m_bInitValueSet = false;

	int m_numElements;
public:
	IMemBuffer(IMemPool* pPool, int numElements) { m_pPool = pPool; m_numElements = numElements; }
	virtual ~IMemBuffer() {};

	virtual double& operator[](size_t index)= 0;
	void setInitValue(double value) { m_initValue = value; m_bInitValueSet = true; }
	bool bInitValueSet() const { return m_bInitValueSet; }
	double getInitValue() const { return m_initValue; }
	int getNumElements() const { return m_numElements; }
	IMemPool* getMemPool() const { return m_pPool; }
};
