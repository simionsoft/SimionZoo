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