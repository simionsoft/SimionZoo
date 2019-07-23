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

#include "deep-minibatch.h"
#include "deep-functions.h"
#include <stdexcept>

DeepMinibatch::DeepMinibatch(size_t minibatchSize, DeepNetworkDefinition* pDeepFunction)
{
	m_pDeepFunction = pDeepFunction;
	m_size = minibatchSize;

	m_s = vector<double>(minibatchSize*pDeepFunction->getInputStateVariables().size());
	m_a = vector<double>(minibatchSize*pDeepFunction->getUsedActionVariables().size());
	m_s_p = vector<double>(minibatchSize*pDeepFunction->getInputStateVariables().size());
	m_r = vector<double>(minibatchSize); //only scalar rewards so far
	m_target = vector<double>(minibatchSize*pDeepFunction->getNumOutputs());
}


DeepMinibatch::~DeepMinibatch()
{
}

void DeepMinibatch::clear()
{
	m_numTuples = 0;
}

void DeepMinibatch::addTuple(const State* s, const Action* a, const State* s_p, double r)
{
	if (m_numTuples >= m_size)
		return;

	addTuple(s, a, s_p, r, m_numTuples);

	m_numTuples++;
}

void DeepMinibatch::addTuple(const State* s, const Action* a, const State* s_p, double r, size_t index)
{
	if (index >= m_size)
		return;

	//copy input state s
	m_pDeepFunction->stateToVector(s, m_s, index);
	m_pDeepFunction->stateToVector(s_p, m_s_p, index);

	//copy input action a
	m_pDeepFunction->actionToVector(a, m_a, index);

	//copy reward r
	m_r[index] = r;
}

void DeepMinibatch::copyElement(vector<double>& src, vector<double>&dst, size_t index)
{
	if (src.size() != dst.size())
		throw new std::runtime_error("copyElement() was called with incompatible vectors");

	// We can assume the vector has been initialized with a multiple of the minibatch size. This multiple
	// is the number of elements in the vector per tuple
	size_t numElementsPerTuple = src.size() / m_size;
	for (size_t i = index * numElementsPerTuple; i < (index + 1)*numElementsPerTuple; i++)
		dst[i] = src[i];
}

bool DeepMinibatch::isFull() const
{
	return m_numTuples == m_size;
}

size_t DeepMinibatch::numTuples() const
{
	return m_numTuples;
}