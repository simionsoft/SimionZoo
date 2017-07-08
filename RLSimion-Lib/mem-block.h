#pragma once
#include "mem-manager.h"
#include <string>
using namespace std;
class CSimionMemPool;

class CMemBlock
{
	CSimionMemPool* m_pPool;
	double* m_pBuffer = nullptr;
	int m_blockSize = 0;
	bool m_bInitialized = false;
	BUFFER_SIZE m_lastAccess = 0;
	int m_id;
	bool m_bDumped = false;

	string getDumpFileName();
public:
	CMemBlock(CSimionMemPool* pPool,int id, int elementCount);
	virtual ~CMemBlock();

	bool bAllocated() const { return m_pBuffer != nullptr; }
	double* deallocate();

	void restoreFromFile();
	void dumpToFile();

	void setBuffer(double* pBuffer);
	int size() const { return m_blockSize; }
	bool bInitialized() const { return m_bInitialized; }
	void setInitialized() { m_bInitialized= true; }
	BUFFER_SIZE getLastAccess() const { return m_lastAccess; }
	void setLastAccess(BUFFER_SIZE newValue) { m_lastAccess = newValue; }
	int getId() const { return m_id; }

	double& operator[](size_t index);
};

