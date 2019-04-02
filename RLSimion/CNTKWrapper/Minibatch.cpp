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

#include "Minibatch.h"
#include "NetworkDefinition.h"

#include <stdexcept>

Minibatch::Minibatch(size_t size, NetworkDefinition* pNetworkDefinition, size_t outputSize)
{
	m_pNetworkDefinition = pNetworkDefinition;
	m_size = size;
	m_s = vector<double>(size*pNetworkDefinition->getInputStateVariables().size());
	size_t numActions = pNetworkDefinition->getInputActionVariables().size();
	if (numActions == 0)
		numActions = 1; //even if the network doesn't use any action as input, we may want to store the action taken by the agent each time-step. Only 1 output is assumed

	m_a = vector<double>(size*numActions);
	m_s_p = vector<double>(size*pNetworkDefinition->getInputStateVariables().size());
	m_r = vector<double>(size);

	if (outputSize == 0)
		//if not overriden, use the network's output size. This is the general case
		m_outputSize = pNetworkDefinition->getOutputSize();
	else
		m_outputSize = outputSize;

	m_target = vector<double>(size*m_outputSize);
}


Minibatch::~Minibatch()
{
}

void Minibatch::clear()
{
	m_numTuples = 0;
}

void Minibatch::addTuple(const State* s, const Action* a, const State* s_p, double r)
{
	if (m_numTuples >= m_size)
		return;

	//copy input state s
	const vector<string>& stateVars= m_pNetworkDefinition->getInputStateVariables();
	size_t stateInputSize = stateVars.size();
	for (size_t i = 0; i < stateInputSize; i++)
	{
		m_s[m_numTuples*stateInputSize + i] = s->getNormalized(stateVars[i].c_str());
		m_s_p[m_numTuples*stateInputSize + i] = s_p->getNormalized(stateVars[i].c_str());
	}

	//copy input action a
	const vector<string>& actionVars = m_pNetworkDefinition->getInputActionVariables();
	size_t actionInputSize = actionVars.size();
	if (actionInputSize > 0)
	{
		for (size_t i = 0; i < actionInputSize; i++)
			m_a[m_numTuples*actionInputSize + i] = a->getNormalized(actionVars[i].c_str());
	}
	else
	{	//no input action, we just save the action selected by the agent
		//for now, we assume only one action
		m_a[m_numTuples] = a->get((size_t) 0);
	}

	//copy reward r
	m_r[m_numTuples] = r;
	
	m_numTuples++;
}

vector<double>& Minibatch::s()
{
	return m_s;
}

vector<double>& Minibatch::a()
{
	return m_a;
}

vector<double>& Minibatch::s_p()
{
	return m_s_p;
}

vector<double>& Minibatch::r()
{
	return m_r;
}

vector<double>& Minibatch::target()
{
	return m_target;
}

void Minibatch::destroy()
{
	delete this;
}

bool Minibatch::isFull() const
{
	return m_numTuples == m_size;
}

size_t Minibatch::size() const
{
	return m_size;
}

size_t Minibatch::outputSize() const
{
	return m_outputSize;
}
