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
#include "noise.h"
#include "featuremap.h"
#include "features.h"
#include "experience-replay.h"
#include "experiment.h"
#include <algorithm>
#include <math.h>

std::shared_ptr<DiscreteDeepPolicy> DiscreteDeepPolicy::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<DiscreteDeepPolicy>(pConfigNode, "Policy", "The policy type",
	{
		{ "Discrete-Epsilon-Greedy-Deep-Policy",CHOICE_ELEMENT_NEW<DiscreteEpsilonGreedyDeepPolicy> },
		{ "Discrete-Softmax-Deep-Policy",CHOICE_ELEMENT_NEW<DiscreteSoftmaxDeepPolicy> },
		{ "Discrete-Exploration", CHOICE_ELEMENT_NEW<DiscreteExplorationDeepPolicy>}
	});
}

DiscreteDeepPolicy::DiscreteDeepPolicy(ConfigNode * pConfigNode)
{	
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
/// <param name="values">Estimated Q(s,a) for each discrete action. Size should equal the number of discrete actions</param>
/// <returns>The index of the selected value</returns>
int DiscreteEpsilonGreedyDeepPolicy::selectAction(const std::vector<double>& values)
{
	double randomValue = getRandomValue();

	size_t resultingActionIndex;
	double eps = m_epsilon->get();

	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
		eps = 0.0;

	if (randomValue < eps)
	{
		resultingActionIndex = std::rand() % static_cast<int>(values.size() + 1);
	}
	else
	{
		resultingActionIndex = std::distance(values.begin(), std::max_element(values.begin(), values.end()));
	}

	return (int) resultingActionIndex;
}

/// <summary>
/// Deep-RL version of the Soft-Max action selection policy
/// </summary>
/// <param name="values">Estimated Q(s,a) for each discrete action. Size should equal the number of discrete actions</param>
/// <returns>The index of the selected value</returns>
int DiscreteSoftmaxDeepPolicy::selectAction(const std::vector<double>& values)
{
	//this is not cool, but we can safely overwrite the output values of the function
	std::vector<double>& newValues = (std::vector<double>&) values;
	double sum = 0;
	for (int i = 0; i < (int)newValues.size(); i++)
	{
		newValues[i] = std::exp(m_temperature->get() * newValues[i]);
		sum += newValues[i];
	}
	for (int i = 0; i < (int) newValues.size(); i++)
	{
		newValues[i] /= sum;
	}
	return chooseRandomInteger(newValues);
}

DiscreteExplorationDeepPolicy::DiscreteExplorationDeepPolicy(ConfigNode * pConfigNode) : DiscreteDeepPolicy(pConfigNode)
{
}

int DiscreteExplorationDeepPolicy::selectAction(const vector<double>& values)
{
	//greedy action policy on evaluation
	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
		return (int) std::distance(values.begin(), std::max_element(values.begin(), values.end()));

	double randomNormNumber = (double)(rand() % 1000) / (double) 1000.0;
	if (randomNormNumber < m_epsilon)
		return m_lastAction; //take the same action with probability epsilon
	if (randomNormNumber < (0.5 *(1.0 + m_epsilon)) && m_lastAction>0) //shift the output to the previous action step with probability (1-epsilon)*0.5
	{
		m_lastAction--;
		return m_lastAction;
	}
	if (m_lastAction < values.size() - 1) //shift the output to the next action
		m_lastAction++;
	return m_lastAction;
}