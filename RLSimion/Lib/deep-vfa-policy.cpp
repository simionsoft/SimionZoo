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

#include "deep-vfa-policy.h"
#include "simgod.h"
#include "app.h"
#include "experiment.h"
#include "noise.h"
#include <algorithm>
#include <math.h>
#include "../Common/named-var-set.h"
#include "deep-functions.h"
#include "deep-network.h"

std::shared_ptr<DiscreteDeepPolicy> DiscreteDeepPolicy::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<DiscreteDeepPolicy>(pConfigNode, "Policy", "The policy type",
	{
		{ "Discrete-Epsilon-Greedy-Deep-Policy",CHOICE_ELEMENT_NEW<DiscreteEpsilonGreedyDeepPolicy> },
		{ "Discrete-Softmax-Deep-Policy",CHOICE_ELEMENT_NEW<DiscreteSoftmaxDeepPolicy> },
		{ "Discrete-Exploration", CHOICE_ELEMENT_NEW<DiscreteExplorationDeepPolicy>},
		{ "Noise-Plus-Greedy-Policy", CHOICE_ELEMENT_NEW<NoisePlusGreedyDeepPolicy>}
	});
}

DiscreteDeepPolicy::DiscreteDeepPolicy(ConfigNode * pConfigNode)
{	
}

void DiscreteDeepPolicy::initialize(DeepDiscreteQFunction* pQFunctionDefinition)
{
	m_pQFunctionDefinition = pQFunctionDefinition;

	m_numSamplesPerActionVariable = pQFunctionDefinition->getNumOutputs();
	if (m_numSamplesPerActionVariable <= 0)
		throw runtime_error("The number of discretized steps must be greater than zero");

	m_argMaxQValues = vector<double>(m_numTotalActionSamples);
	m_stateVector = vector<double>(pQFunctionDefinition->getInputStateVariables().size());
	m_argMaxAction = vector<double>(pQFunctionDefinition->getNumOutputActions());
	m_outputActionVariables = pQFunctionDefinition->getOutputActionVariables();

	//We use normalized values, so there is no need to replicate the same vector for all the action variables
	m_discretizedAction = vector<double>(m_numSamplesPerActionVariable);
	for (int sample = 0; sample < m_numSamplesPerActionVariable; sample++)
		m_discretizedAction[sample] = (double)sample / double(m_numSamplesPerActionVariable - 1);
}

//This method works with normalized values so there is no need to denormalize them
size_t DiscreteDeepPolicy::getActionIndex(const vector<double>& action, int actionOffset)
{
	size_t actionVarIndex;
	size_t actionIndex = 0;
	for (size_t i = 0; i < m_outputActionVariables.size(); i++)
	{
		actionVarIndex = getActionVariableIndex(action[actionOffset*m_outputActionVariables.size() + i]);
		actionIndex = actionIndex * m_numSamplesPerActionVariable + actionVarIndex;
	}
	return actionIndex;
}

//This method works with normalized values so there is no need to denormalize them
size_t DiscreteDeepPolicy::getActionVariableIndex(double value)
{
	size_t nearestIndex = 0;

	double dist;
	double closestDist = abs(value - m_discretizedAction[0]);

	for (size_t i = 1; i < m_discretizedAction.size(); i++)
	{
		dist = abs(value - m_discretizedAction[i]);
		//there is no special treatment for circular variables 
		if (dist < closestDist)
		{
			closestDist = dist;
			nearestIndex = i;
		}
	}

	return nearestIndex;
}


void DiscreteDeepPolicy::greedyActionSelection(IDiscreteQFunctionNetwork* pNetwork, const State* s, Action* a)
{
	vector<double> qValues = pNetwork->evaluate(s, a);

	size_t maxQActionIndex= std::distance(qValues.begin(), std::max_element(qValues.begin(), qValues.end()));

	size_t actionVarIndex;

	for (int i = m_outputActionVariables.size() -1; i >=0 ; i--)
	{
		actionVarIndex = maxQActionIndex % m_numSamplesPerActionVariable;
		a->setNormalized( m_outputActionVariables[i].c_str(), m_discretizedAction[actionVarIndex] );
		maxQActionIndex /= m_numSamplesPerActionVariable;
	}
}

void DiscreteDeepPolicy::randomActionSelection(IDiscreteQFunctionNetwork* pNetwork, const State* s, Action* a)
{
	double randomActionValue, aMin, aMax, aRange;
	const char* actionVariableName;
	const vector<string>& actionVariables = pNetwork->getInputActionVariables();
	for (int i = 0; i < actionVariables.size(); i++)
	{
		actionVariableName = actionVariables[i].c_str();
		aMin= a->getDescriptor().getProperties(actionVariableName)->getMin();
		aMax = a->getDescriptor().getProperties(actionVariableName)->getMax();
		aRange = aMax - aMin;
		randomActionValue = aMin + aRange * getRandomValue();
		a->set(pNetwork->getInputActionVariables()[i].c_str(), randomActionValue);
	}
}


DiscreteEpsilonGreedyDeepPolicy::DiscreteEpsilonGreedyDeepPolicy(ConfigNode * pConfigNode) : DiscreteDeepPolicy(pConfigNode)
{
	m_epsilon = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "epsilon", "Epsilon");
}

DiscreteSoftmaxDeepPolicy::DiscreteSoftmaxDeepPolicy(ConfigNode * pConfigNode) : DiscreteDeepPolicy(pConfigNode)
{
	m_temperature = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "temperature", "Temperature");
}

/// <summary>
/// Deep RL version of the epsilon-greedy action selection algorithm
/// </summary>
/// <param name="values">Network used to represent Q(s,a). Both the state and action should be inputs to the network for discrete-output learners such as DQN</param>
/// <returns>The index of the selected value</returns>
void DiscreteEpsilonGreedyDeepPolicy::selectAction(IDiscreteQFunctionNetwork* pNetwork, const State* s, Action* a)
{
	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//Greedy selection
		greedyActionSelection(pNetwork, s, a);
		return;
	}

	double randomValue = getRandomValue();
	double eps = m_epsilon->get();

	if (randomValue < eps)
		randomActionSelection(pNetwork, s, a);
	else
		greedyActionSelection(pNetwork, s, a);
}

/// <summary>
/// Deep-RL version of the Soft-Max action selection policy
/// </summary>
/// <param name="values">Estimated Q(s,a) for each discrete action. Size should equal the number of discrete actions</param>
/// <returns>The index of the selected value</returns>
void DiscreteSoftmaxDeepPolicy::selectAction(IDiscreteQFunctionNetwork* pNetwork, const State* s, Action* a)
{
	//this is not cool, but we can safely overwrite the output values of the function
	//std::vector<double>& newValues = (std::vector<double>&) values;
	//double sum = 0;
	//for (int i = 0; i < (int)newValues.size(); i++)
	//{
	//	newValues[i] = std::exp(m_temperature->get() * newValues[i]);
	//	sum += newValues[i];
	//}
	//for (int i = 0; i < (int) newValues.size(); i++)
	//{
	//	newValues[i] /= sum;
	//}
	//return chooseRandomInteger(newValues);
}

DiscreteExplorationDeepPolicy::DiscreteExplorationDeepPolicy(ConfigNode * pConfigNode) : DiscreteDeepPolicy(pConfigNode)
{
}

void DiscreteExplorationDeepPolicy::selectAction(IDiscreteQFunctionNetwork* pNetwork, const State* s, Action* a)
{
	//greedy action policy on evaluation
	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//Greedy selection
		greedyActionSelection(pNetwork, s, a);
		return;
	}

	//shift the action variables up or down one by one
	for (int i = 0; i < m_outputActionVariables.size(); i++)
	{
		const char* actionVariableName= m_outputActionVariables[i].c_str();
		double currentValue = a->get(actionVariableName);
		double randomNormNumber = (double)(rand() % 1000) / (double) 1000.0;
		//leave the output action unchanged with probability epsilon
		if (randomNormNumber > m_epsilon)
		{
			if (randomNormNumber < (0.5 *(1.0 + m_epsilon))) //shift the output to the next action step with probability (1-epsilon)*0.5
				a->set(actionVariableName, currentValue + (1.0 / m_numSamplesPerActionVariable)); //shift the current value of the variable to the next step
			else
				a->set(actionVariableName, currentValue - (1.0 / m_numSamplesPerActionVariable)); //shift the current value of the variable to the previous step
		}
	}
}

NoisePlusGreedyDeepPolicy::NoisePlusGreedyDeepPolicy(ConfigNode* pConfigNode) : DiscreteDeepPolicy(pConfigNode)
{
	m_noiseSignals = MULTI_VALUE_FACTORY<Noise>(pConfigNode, "Exploration-Noise", "Noise signals added to each of the outputs of the greedy policy");
}

void NoisePlusGreedyDeepPolicy::selectAction(IDiscreteQFunctionNetwork* pNetwork, const State* s, Action* a)
{
	//if it's an evaluation episode, nothing else to be done: just return the greedy output
	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//Greedy selection
		greedyActionSelection(pNetwork, s, a);
		return;
	}

	double noise;
	size_t noiseSignalIndex;
	for (size_t i = 0; i < m_outputActionVariables.size(); i++)
	{
		//if there are less noise signals than output action variables, just use the last one
		noiseSignalIndex = std::min(i, m_noiseSignals.size() - 1);
		
		noise = m_noiseSignals[i]->getSample();
		a->set(m_outputActionVariables[i].c_str(), a->get(m_outputActionVariables[i].c_str()) + noise);
	}
}