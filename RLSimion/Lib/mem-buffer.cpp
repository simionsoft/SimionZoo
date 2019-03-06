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

#include "mem-pool.h"
#include "mem-block.h"
#include "mem-buffer.h"


SimpleMemBuffer::SimpleMemBuffer(IMemPool* pPool, BUFFER_SIZE elementCount)
	:m_pPool(pPool), m_blockSize(elementCount),IMemBuffer(pPool,elementCount)
{
	m_pBuffer = new double[elementCount];
	pPool->updateTotalMemAllocated(elementCount * sizeof(double));
}
SimpleMemBuffer::~SimpleMemBuffer()
{
	if (m_pBuffer != nullptr) delete[] m_pBuffer;
}


double& SimpleMemBuffer::operator[](BUFFER_SIZE index)
{
	return m_pBuffer[index];
}




SimionMemBuffer::SimionMemBuffer(SimionMemPool* pPool
	, BUFFER_SIZE elementCount, BUFFER_SIZE elementSize, BUFFER_SIZE offset)
	:m_pPool(pPool), m_elementSize(elementSize), m_offset(offset),IMemBuffer(pPool,elementCount)
{

}

SimionMemBuffer::~SimionMemBuffer()
{
}

/// <summary>
/// Accessor method
/// </summary>
/// <param name="index">Index of the element</param>
/// <returns>A reference to the element that can be used to read or write the element</returns>
double& SimionMemBuffer::operator[](BUFFER_SIZE index)
{
	return m_pPool->get((int)index,m_offset);
}

/// <summary>
/// Returns the size of a memory block in the parent memory pool
/// </summary>
/// <returns></returns>
BUFFER_SIZE SimionMemBuffer::getBlockSizeInBytes()
{
	return m_pPool->getBlockSize()*sizeof(double);
}