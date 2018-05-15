#ifdef _WIN64

#include "deep-vfa-policy.h"
#include "parameters.h"
#include "SimGod.h"
#include "app.h"
#include "noise.h"
#include "featuremap.h"
#include "features.h"
#include "experience-replay.h"
#include "experiment.h"
#include <algorithm>

std::shared_ptr<DiscreteDeepPolicy> DiscreteDeepPolicy::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<DiscreteDeepPolicy>(pConfigNode, "Policy", "The policy type",
	{
		{ "Discrete-Epsilon-Greedy-Deep-Policy",CHOICE_ELEMENT_NEW<DiscreteEpsilonGreedyDeepPolicy> },
		{ "Discrete-Softmax-Deep-Policy",CHOICE_ELEMENT_NEW<DiscreteSoftmaxDeepPolicy> }
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

int DiscreteSoftmaxDeepPolicy::selectAction(const std::vector<double>& values)
{
	//this is not cool, but we can safely overwrite the output values of the function
	std::vector<double>& newValues = (std::vector<double>&) values;
	double sum = 0;
	for (int i = 0; i < newValues.size(); i++)
	{
		newValues[i] = std::exp(m_temperature->get() * newValues[i]);
		sum += newValues[i];
	}
	for (int i = 0; i < newValues.size(); i++)
	{
		newValues[i] /= sum;
	}
	return chooseRandomInteger(newValues);
}

#endif