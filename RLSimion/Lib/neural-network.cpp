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

#include "neural-network.h"
/*
#include <algorithm>
#include "../tools/CNTKWrapper/CNTKWrapper.h"

NeuralNetwork::NeuralNetwork(INetwork* pNetwork, vector<string>& inputStateVariables, vector<string>& inputActionVariables, int outputSize)
{
	m_pNetwork = pNetwork;
	for each (string variable in inputStateVariables)
		m_inputStateVariables.push_back(variable);

	for each (string variable in inputActionVariables)
		m_inputActionVariables.push_back(variable);

	m_inputSize = inputStateVariables.size() + inputActionVariables.size();
	m_outputSize = outputSize;
}

NeuralNetwork::~NeuralNetwork()
{
	if (m_pNetwork)
		m_pNetwork->destroy();
}


size_t NeuralNetwork::getInputSize()
{
	return m_inputSize;
}

size_t NeuralNetwork::getOutputSize()
{
	return m_outputSize;
}


NNQFunctionDiscreteActions::NNQFunctionDiscreteActions(INetwork* pNetwork, vector<string> &inputStateVariables, string actionVariableName, size_t numSteps, double minvalue, double maxvalue)
	: NeuralNetwork(pNetwork)
{

	for (size_t i = 0; i < inputStateVariables.size(); ++i)
		addInputStateVariable(inputStateVariables[i]);

	double stepSize;

	m_outputSize = numSteps;

	m_outputActionValues = vector<double>(numSteps);
	for (size_t i = 0; i < numSteps; i++)
	{
		stepSize = (maxvalue - minvalue) / ((int)numSteps - 1);
		m_outputActionValues[i] = minvalue + stepSize * i;
	}
}

double NNQFunctionDiscreteActions::getActionIndexOutput(size_t actionIndex)
{
	actionIndex = std::max((size_t)0, std::min(m_outputActionValues.size() - 1, actionIndex));
	return m_outputActionValues[actionIndex];
}

size_t NNQFunctionDiscreteActions::getClosestOutputIndex(double value)
{
	size_t nearestIndex = 0;

	double dist;
	double closestDist = abs(value - m_outputActionValues[0]);

	for (size_t i = 1; i < m_outputActionValues.size(); i++)
	{
		dist = abs(value - m_outputActionValues[i]);
		//there is no special treatment for circular variables 
		if (dist < closestDist)
		{
			closestDist = dist;
			nearestIndex = i;
		}
	}

	return nearestIndex;
}

NNQFunctionDiscreteActions* NNQFunctionDiscreteActions::clone()
{
	INetwork* pNetworkClone = m_pNetwork->clone();
	NNQFunctionDiscreteActions* pNewNetwork = new NNQFunctionDiscreteActions();
	pNewNetwork->m_pNetwork = pNetworkClone;
}

NNQFunction::NNQFunction(INetwork* pNetwork, vector<string> &inputStateVariables, vector<string> &inputActionVariables)
	: NeuralNetwork(pNetwork)
{

	for (size_t i = 0; i < inputStateVariables.size(); ++i)
		addInputStateVariable(inputStateVariables[i]);
	m_inputStateBuffer = vector<double>(inputStateVariables.size());

	for (size_t i = 0; i < inputActionVariables.size(); ++i)
		addInputActionVariable(inputActionVariables[i]);

	m_outputSize = 1;
}

double NNQFunctionDiscreteActions::getActionIndexOutput(size_t actionIndex)
{
	actionIndex = std::max((size_t)0, std::min(m_outputActionValues.size() - 1, actionIndex));
	return m_outputActionValues[actionIndex];
}

size_t NNQFunctionDiscreteActions::getClosestOutputIndex(double value)
{
	size_t nearestIndex = 0;

	double dist;
	double closestDist = abs(value - m_outputActionValues[0]);

	for (size_t i = 1; i < m_outputActionValues.size(); i++)
	{
		dist = abs(value - m_outputActionValues[i]);
		//there is no special treatment for circular variables 
		if (dist < closestDist)
		{
			closestDist = dist;
			nearestIndex = i;
		}
	}

	return nearestIndex;
}

NNQFunctionDiscreteActions* NNQFunctionDiscreteActions::clone()
{
	INetwork* pNetworkClone = m_pNetwork->clone();
	NNQFunctionDiscreteActions* pNewNetwork = new NNQFunctionDiscreteActions();
	pNewNetwork->m_pNetwork = pNetworkClone;
}
*/