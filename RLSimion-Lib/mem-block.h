#pragma once

class CInterleavedMemPool;

class CMemBlock
{
	CInterleavedMemPool* m_pPool;
	double* m_pBuffer = nullptr;
	int m_blockSizeInBytes = 0;
	bool bLazyAllocation = false;
public:
	CMemBlock(CInterleavedMemPool* pPool, int elementCount);
	virtual ~CMemBlock();

	double& operator[](size_t index);
};

