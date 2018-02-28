#include "mem-block.h"
#include "mem-pool.h"

CMemBlock::CMemBlock(CSimionMemPool* pPool, int id, size_t blockSize)
	: m_pPool(pPool), m_blockSize(blockSize), m_id (id)
{
}

CMemBlock::~CMemBlock()
{
	if (m_pBuffer != nullptr)
		delete[] m_pBuffer;
}

double& CMemBlock::operator[](size_t index)
{
	m_lastAccess = m_pPool->getAccessCounter();
	if (m_lastAccess == std::numeric_limits<BUFFER_SIZE>::max())
	{
		m_pPool->resetAccessCounter();
	}
	return m_pBuffer[index];
}

double* CMemBlock::deallocate()
{
	double* pBuffer = m_pBuffer;
	m_pBuffer = 0;
	m_bInitialized = true; //must be restored from file
	return pBuffer;
}

void CMemBlock::dumpToFile()
{
	FILE* pFile;

	if (!m_pBuffer)
		return;

	string dumpFile = getDumpFileName();
	fopen_s(&pFile, dumpFile.c_str(), "wb");
	if (pFile)
	{
		m_bDumped = true;
		fwrite(m_pBuffer, sizeof(double), m_blockSize, pFile);
		fclose(pFile);
	}
}

void CMemBlock::restoreFromFile()
{
	if (!m_bDumped) return;
	
	FILE* pFile;
	string dumpFile = getDumpFileName();
	fopen_s(&pFile, dumpFile.c_str(), "rb");
	if (pFile)
	{
		m_bDumped = false;
		fread_s(m_pBuffer,sizeof(double)*m_blockSize, sizeof(double), m_blockSize, pFile);
		fclose(pFile);
	}
}

void CMemBlock::setBuffer(double *pMemBuffer)
{
	m_pBuffer = pMemBuffer;
}

string CMemBlock::getDumpFileName()
{
	return string("mem-dump.") + std::to_string(m_id) + string(".tmp");
}