/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "mem-block.h"
#include "mem-pool.h"
#include "../../tools/System/CrossPlatform.h"
#include <limits>


/// <summary>
/// MemBlock constructor. Each memory pool contains several interleaved buffers of the same size and it 
/// is divided in smaller blocks. This object class represents a memory block that holds a part
/// </summary>
/// <param name="pPool">Parent pool</param>
/// <param name="id">Id of this MemBlock</param>
/// <param name="blockSize">Size of each block in this pool (in elements)</param>
MemBlock::MemBlock(SimionMemPool* pPool, int id, size_t blockSize)
	: m_pPool(pPool), m_blockSize(blockSize), m_id (id)
{
}

MemBlock::~MemBlock()
{
	if (m_pBuffer != nullptr)
		delete[] m_pBuffer;
}

double& MemBlock::operator[](size_t index)
{
	m_lastAccess = m_pPool->getAccessCounter();
	if (m_lastAccess == std::numeric_limits<BUFFER_SIZE>::max())
	{
		m_pPool->resetAccessCounter();
	}
	return m_pBuffer[index];
}

double* MemBlock::deallocate()
{
	double* pBuffer = m_pBuffer;
	m_pBuffer = 0;
	m_bInitialized = true; //must be restored from file
	return pBuffer;
}

/// <summary>
/// Saves the contents of the memory block to a temporary file.
/// </summary>
void MemBlock::dumpToFile()
{
	FILE* pFile;

	if (!m_pBuffer)
		return;

	string dumpFile = getDumpFileName();
	CrossPlatform::Fopen_s(&pFile, dumpFile.c_str(), "wb");
	if (pFile)
	{
		m_bDumped = true;
		fwrite(m_pBuffer, sizeof(double), m_blockSize, pFile);
		fclose(pFile);
	}
}

/// <summary>
/// Restores the contents of a memory block from file
/// </summary>
void MemBlock::restoreFromFile()
{
	if (!m_bDumped) return;
	
	FILE* pFile;
	string dumpFile = getDumpFileName();
	CrossPlatform::Fopen_s(&pFile, dumpFile.c_str(), "rb");
	if (pFile)
	{
		m_bDumped = false;
		CrossPlatform::Fread_s(m_pBuffer,sizeof(double)*m_blockSize, sizeof(double), m_blockSize, pFile);
		fclose(pFile);
	}
}

void MemBlock::setBuffer(double *pMemBuffer)
{
	m_pBuffer = pMemBuffer;
}

string MemBlock::getDumpFileName()
{
	return string("mem-dump.") + std::to_string(m_id) + string(".tmp");
}