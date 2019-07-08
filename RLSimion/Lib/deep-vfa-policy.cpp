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
#include "parameters.h"
#include "simgod.h"
#include "app.h"
#include "experiment.h"
#include "noise.h"
#include <algorithm>
#include <math.h>
#include "../Common/named-var-set.h"
#include "../CNTKWrapper/CNTKWrapper.h"

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

void DiscreteDeepPolicy::initialize(MULTI_VALUE<STATE_VARIABLE>& inputState, MULTI_VALUE<ACTION_VARIABLE>& outputAction, int numSamplesPerActionVariable)
{
	m_numSamplesPerActionVariable = numSamplesPerActionVariable;
	m_numActionVariables = (int) outputAction.size();
	m_outputActionVariables = vector<string>(m_numActionVariables);
	m_numTotalActionSamples = 1;
	for (int i = 0; i < m_numActionVariables; i++)
	{
		m_numTotalActionSamples *= m_numSamplesPerActionVariable;
		m_outputActionVariables[i] = outputAction[i]->get();
	}
	m_argMaxQValues = vector<double>(m_numTotalActionSamples);
	m_stateVector = vector<double>(inputState.size());
	m_argMaxAction = vector<double>(outputAction.size());

	//We use normalized values, so there is no need to replicate the same vector for all the action variables
	m_discretizedAction = vector<double>(m_numSamplesPerActionVariable);
	for (int sample = 0; sample < m_numSamplesPerActionVariable; sample++)
	{
		if (m_numSamplesPerActionVariable > 0)
			m_discretizedAction[sample] = (double)sample / double(m_numSamplesPerActionVariable - 1);
		else
			throw runtime_error("The number of discretized steps must be greater than zero");
	}
}

//This method works with normalized values so there is no need to denormalize them
size_t DiscreteDeepPolicy::getActionIndex(const vector<double>& action, int actionOffset)
{
	size_t actionVarIndex;
	size_t actionIndex = 0;
	for (size_t i = 0; i < m_numActionVariables; i++)
	{
		actionVarIndex = getActionVariableIndex(action[actionOffset*m_numActionVariables + i]);
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


void DiscreteDeepPolicy::greedyActionSelection(INetwork* pNetwork, const State* s, Action* a)
{
	pNetwork->getDefinition()->inputStateVariablesToVector(s, m_stateVector, 0);

	pNetwork->evaluate(m_stateVector, m_argMaxAction, m_argMaxQValues);

	size_t maxQActionIndex= std::distance(m_argMaxQValues.begin(), std::max_element(m_argMaxQValues.begin(), m_argMaxQValues.end()));

	size_t actionVarIndex;

	for (int i = m_numActionVariables -1; i >=0 ; i--)
	{
		actionVarIndex = maxQActionIndex % m_numSamplesPerActionVariable;
		a->setNormalized( m_outputActionVariables[i].c_str(), m_discretizedAction[actionVarIndex] );
		maxQActionIndex /= m_numSamplesPerActionVariable;
	}
}

void DiscreteDeepPolicy::randomActionSelection(INetwork* pNetwork, const State* s, Action* a)
{
	double randomActionValue, aMin, aMax, aRange;
	const char* actionVariableName;
	const vector<string>& actionVariables = pNetwork->getDefinition()->getInputActionVariables();
	for (int i = 0; i < actionVariables.size(); i++)
	{
		actionVariableName = actionVariables[i].c_str();
		aMin= a->getDescriptor().getProperties(actionVariableName)->getMin();
		aMax = a->getDescriptor().getProperties(actionVariableName)->getMax();
		aRange = aMax - aMin;
		randomActionValue = aMin + aRange * getRandomValue();
		a->set(pNetwork->getDefinition()->getInputActionVariables()[i].c_str(), randomActionValue);
	}
}


DiscreteEpsilonGreedyDeepPolicy::DiscreteEpsilonGreedyDeepPolicy(ConfigNode * pConfigNode) : DiscreteDeepPolicy(pConfigNode)
{
	m_epsilon = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "epsilon", "Epsilon");
}

DiscreteSoftmaxDeepPolicy::DiscreteSoftmaxDeepPolicy(ConfigNode * pConfigNode) : DiscreteDeepPolicy(pConfigNode)
{
	m_temperature = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "temperature", "Tempreature");
}

/// <summary>
/// Deep RL version of the epsilon-greedy action selection algorithm
/// </summary>
/// <param name="values">Network used to represent Q(s,a). Both the state and action should be inputs to the network for discrete-output learners such as DQN</param>
/// <returns>The index of the selected value</returns>
void DiscreteEpsilonGreedyDeepPolicy::selectAction(INetwork* pNetwork, const State* s, Action* a)
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
void DiscreteSoftmaxDeepPolicy::selectAction(INetwork* pNetwork, const State* s, Action* a)
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

void DiscreteExplorationDeepPolicy::selectAction(INetwork* pNetwork, const State* s, Action* a)
{
	//greedy action policy on evaluation
	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//Greedy selection
		greedyActionSelection(pNetwork, s, a);
		return;
	}

	//double randomNormNumber = (double)(rand() % 1000) / (double) 1000.0;
	//if (randomNormNumber < m_epsilon)
	//	return m_lastAction; //take the same action with probability epsilon
	//if (randomNormNumber < (0.5 *(1.0 + m_epsilon)) && m_lastAction>0) //shift the output to the previous action step with probability (1-epsilon)*0.5
	//{
	//	m_lastAction--;
	//	return m_lastAction;
	//}
	//if (m_lastAction < (int) values.size() - 1) //shift the output to the next action
	//	m_lastAction++;
	//return m_lastAction;
}

NoisePlusGreedyDeepPolicy::NoisePlusGreedyDeepPolicy(ConfigNode* pConfigNode) : DiscreteDeepPolicy(pConfigNode)
{
	m_noiseSignals = MULTI_VALUE_FACTORY<Noise>(pConfigNode, "Exploration-Noise", "Noise signals added to each of the outputs of the greedy policy");
}

void NoisePlusGreedyDeepPolicy::selectAction(INetwork* pNetwork, const State* s, Action* a)
{
	//if it's an evaluation episode, nothing else to be done: just return the greedy output
	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//Greedy selection
		greedyActionSelection(pNetwork, s, a);
		return;
	}

	const vector<string>& outputActions = pNetwork->getDefinition()->getInputActionVariables();
	double noise;
	for (int i = 0; i < outputActions.size(); i++)
	{
		if (i < m_noiseSignals.size())
		{
			noise = m_noiseSignals[i]->getSample();
			a->set(outputActions[i].c_str(),/* a->get(outputActions[i].c_str()) + */noise);
		}
	}
}