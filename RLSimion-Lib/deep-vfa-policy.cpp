#ifdef _WIN64

#include "deep-vfa-policy.h"
#include "parameters.h"
#include "SimGod.h"
#include "app.h"
#include "noise.h"
#include "featuremap.h"
#include "features.h"
#include "single-dimension-discrete-grid.h"
#include "experience-replay.h"

std::shared_ptr<CDiscreteDeepPolicy> CDiscreteDeepPolicy::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE<CDiscreteDeepPolicy>(pConfigNode, "Policy", "The policy type",
	{
		{ "Discrete-Epsilon-Greedy-Deep-Policy",CHOICE_ELEMENT_NEW<CDiscreteEpsilonGreedyDeepPolicy> },
		{ "Discrete-Softmax-Deep-Policy",CHOICE_ELEMENT_NEW<CDiscreteSoftmaxDeepPolicy> }
	});
}

CDiscreteDeepPolicy::CDiscreteDeepPolicy(CConfigNode * pConfigNode)
{	
}

CDiscreteEpsilonGreedyDeepPolicy::CDiscreteEpsilonGreedyDeepPolicy(CConfigNode * pConfigNode) : CDiscreteDeepPolicy(pConfigNode)
{
	m_epsilon = CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "epsilon", "Epsilon");
}

CDiscreteSoftmaxDeepPolicy::CDiscreteSoftmaxDeepPolicy(CConfigNode * pConfigNode) : CDiscreteDeepPolicy(pConfigNode)
{
	m_temperature = CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "temperature", "Tempreature");
}

int CDiscreteEpsilonGreedyDeepPolicy::selectAction(std::vector<float> values)
{
	double randomValue = getRandomValue();

	int resultingActionIndex;
	double eps = m_epsilon->get();

	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
		eps = 0.0;

	if (randomValue < eps)
	{
		resultingActionIndex = std::rand() % static_cast<int>(values.size() + 1);
	}
	else
	{
		resultingActionIndex = std::distance(values.begin(), std::max_element(values.begin(), values.end()));
	}

	return resultingActionIndex;
}

int CDiscreteSoftmaxDeepPolicy::selectAction(std::vector<float> values)
{
	double sum = 0;
	for (int i = 0; i < values.size(); i++)
	{
		values[i] = std::exp(m_temperature->get() * values[i]);
		sum += values[i];
	}
	for (int i = 0; i < values.size(); i++)
	{
		values[i] /= sum;
	}
	return chooseRandomInteger(values);
}

#endif