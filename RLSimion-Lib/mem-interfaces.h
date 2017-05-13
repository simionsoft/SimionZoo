#pragma once

class IMemBuffer;

class IMemPool
{
	int m_totalAllocatedMem = 0;
public:
	virtual ~IMemPool() {};

	virtual IMemBuffer* getHandler(int elementCount)= 0;
	virtual void init(int blockSize= 524288) = 0;
	virtual bool bCanAllocate(int elementCount) const = 0;

	int getTotalAllocatedMem() const { return m_totalAllocatedMem; }
	void updateTotalMemAllocated(int inc) { m_totalAllocatedMem += inc; }
};

class IMemBuffer
{
	double m_initValue;
public:
	virtual ~IMemBuffer() {};

	virtual double& operator[](size_t index)= 0;
	void setInitValue(double value) { m_initValue = value; }
	double getInitValue() const { return m_initValue; }
};
