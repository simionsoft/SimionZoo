#pragma once
#include "mem-manager.h"
#include <string>
using namespace std;
class SimionMemPool;

class MemBlock
{
	SimionMemPool* m_pPool;
	double* m_pBuffer = nullptr;
	size_t m_blockSize = 0;
	bool m_bInitialized = false;
	BUFFER_SIZE m_lastAccess = 0;
	int m_id;
	bool m_bDumped = false;

	string getDumpFileName();
public:
	MemBlock(SimionMemPool* pPool,int id, size_t elementCount);
	virtual ~MemBlock();

	bool bAllocated() const { return m_pBuffer != nullptr; }
	double* deallocate();

	void restoreFromFile();
	void dumpToFile();

	void setBuffer(double* pBuffer);
	size_t size() const { return m_blockSize; }
	bool bInitialized() const { return m_bInitialized; }
	void setInitialized() { m_bInitialized= true; }
	BUFFER_SIZE getLastAccess() const { return m_lastAccess; }
	void setLastAccess(BUFFER_SIZE newValue) { m_lastAccess = newValue; }
	int getId() const { return m_id; }

	double& operator[](size_t index);
};

