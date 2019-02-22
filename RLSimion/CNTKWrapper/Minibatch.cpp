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
	m_inputState = vector<double>(size*pNetworkDefinition->getInputStateVariables().size());
	m_inputAction = vector<double>(size*pNetworkDefinition->getInputActionVariables().size());

	if (outputSize == 0)
		//if not overriden, use the network's output size. This is the general case
		m_outputSize = pNetworkDefinition->getOutputSize();
	else
		m_outputSize = outputSize;

	m_output = vector<double>(size*m_outputSize);
}


Minibatch::~Minibatch()
{
}

void Minibatch::clear()
{
	m_numTuples = 0;
}

//Added for convenience to avoid vectors on the DLL client side when possible
void Minibatch::addTuple(const State* s, const Action* a, double targetValue)
{
	if (m_outputSize != 1)
		throw std::runtime_error("Cannot use a scalar target value with multiple-output networks");

	vector<double> targetValues = vector<double>(1);
	targetValues[0] = targetValue;
	addTuple(s, a, targetValues);
}

void Minibatch::addTuple(const State* s, const Action* a, const vector<double>& targetValues)
{
	if (m_numTuples >= m_size)
		return;

	if (targetValues.size() != m_outputSize)
		throw std::runtime_error("Missmatched tuple output size and minibatch output size");

	//copy state input
	const vector<string>& stateVars= m_pNetworkDefinition->getInputStateVariables();
	size_t stateInputSize = stateVars.size();
	for (size_t i= 0; i<stateInputSize; i++)
		m_inputState[m_numTuples*stateInputSize + i] = s->getNormalized(stateVars[i].c_str());

	//copy action input
	const vector<string>& actionVars = m_pNetworkDefinition->getInputActionVariables();
	size_t actionInputSize = actionVars.size();
	for (size_t i = 0; i<actionInputSize; i++)
		m_inputAction[m_numTuples*actionInputSize + i] = a->getNormalized(actionVars[i].c_str());

	//copy target values
	for (size_t i = 0; i < targetValues.size(); i++)
		m_output[m_numTuples*m_outputSize + i] = targetValues[i];
	
	m_numTuples++;
}

vector<double>& Minibatch::getInputState()
{
	return m_inputState;
}

vector<double>& Minibatch::getInputAction()
{
	return m_inputAction;
}

vector<double>& Minibatch::getOutput()
{
	return m_output;
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
