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

CDeepPolicy::CDeepPolicy(CConfigNode* pConfigNode)
{
	m_QNetwork = NEURAL_NETWORK_PROBLEM_DESCRIPTION(pConfigNode, "neural-network", "Neural Network Architecture");
	m_outputActionIndex = ACTION_VARIABLE(pConfigNode, "Output-Action", "The output action variable");
}

CDeepPolicy::~CDeepPolicy()
{
}

std::shared_ptr<CDeepPolicy> CDeepPolicy::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE<CDeepPolicy>(pConfigNode, "Policy", "The policy type",
	{
		{ "Discrete-Epsilon-Greedy-Deep-Policy",CHOICE_ELEMENT_NEW<CDiscreteEpsilonGreedyDeepPolicy> },
		{ "Discrete-Softmax-Deep-Policy",CHOICE_ELEMENT_NEW<CDiscreteSoftmaxDeepPolicy> }
	});
}

CDiscreteDeepPolicy::CDiscreteDeepPolicy(CConfigNode * pConfigNode) : CDeepPolicy(pConfigNode)
{
	m_pStateOutFeatures = new CFeatureList("state-input");

	m_numberOfStateFeatures = CSimGod::getGlobalStateFeatureMap()->getTotalNumFeatures();

	if (m_numberOfStateFeatures == 0)
		CLogger::logMessage(MessageType::Error, "Invalid State-Map chosen (it will return 0 features at most, but at least 1 has to be returned)");

	if (dynamic_cast<CDiscreteActionFeatureMap*>(CSimGod::getGlobalActionFeatureMap().get()) == nullptr)
	{
		CLogger::logMessage(MessageType::Error, "The CDiscreteEpsilonGreedyDeepPolicy requires a CDiscreteActionFeatureMap as the action-feature-map.");
	}

	m_pGrid = ((CSingleDimensionDiscreteActionVariableGrid*)(((CDiscreteActionFeatureMap*)
		CSimGod::getGlobalActionFeatureMap().get())->returnGrid()[m_outputActionIndex.get()]));

	if (m_pGrid->getNumCenters() != m_QNetwork.getNetwork()->getTargetOutput().Shape().TotalSize())
	{
		CLogger::logMessage(MessageType::Error, "Output of the network has not the same size as the discrete action grid has centers/discrete values");
	}

	m_numberOfActions = m_QNetwork.getNetwork()->getTargetOutput().Shape().TotalSize();
	m_stateVector = std::vector<float>(m_numberOfStateFeatures, 0.0);
	m_actionValuePredictionVector = std::vector<float>(m_numberOfActions);
}

CDiscreteEpsilonGreedyDeepPolicy::CDiscreteEpsilonGreedyDeepPolicy(CConfigNode * pConfigNode) : CDiscreteDeepPolicy(pConfigNode)
{
	m_epsilon = CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "epsilon", "Epsilon");
}

double CDiscreteEpsilonGreedyDeepPolicy::selectAction(const CState * s, CAction * a)
{
	double randomValue = getRandomValue();

	int resultingActionIndex;
	double eps = m_epsilon->get();

	if (CSimionApp::get()->pExperiment->isEvaluationEpisode())
		eps = 0.0;

	if (randomValue < eps)
	{
		resultingActionIndex = std::rand() % static_cast<int>(m_numberOfActions + 1);
	}
	else
	{
		CSimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(s, m_pStateOutFeatures);

		//TODO: use sparse representation
		for (int i = 0; i < m_pStateOutFeatures->m_numFeatures; i++)
		{
			auto item = m_pStateOutFeatures->m_pFeatures[i];
			m_stateVector[item.m_index] = item.m_factor;
		}

		std::unordered_map<std::string, std::vector<float>&> inputMap = { { "state-input", m_stateVector } };

		m_QNetwork.getNetwork()->predict(inputMap, m_actionValuePredictionVector);
		resultingActionIndex = std::distance(m_actionValuePredictionVector.begin(),
			std::max_element(m_actionValuePredictionVector.begin(), m_actionValuePredictionVector.end()));
	}

	double actionValue = m_pGrid->getCenters()[resultingActionIndex];
	a->set(m_outputActionIndex.get(), actionValue);

	return 1.0;
}

double CDiscreteEpsilonGreedyDeepPolicy::updateNetwork(const CState * s, const CAction * a, const CState * s_p, double r)
{
	double gamma = CSimionApp::get()->pSimGod->getGamma();

	//m_experienceReplay->addTuple(s, a, s_p, r, 1.0);

	CSimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(s, m_pStateOutFeatures);

	//TODO: use sparse representation
	for (int i = 0; i < m_pStateOutFeatures->m_numFeatures; i++)
	{
		auto item = m_pStateOutFeatures->m_pFeatures[i];
		m_stateVector[item.m_index] = item.m_factor;
	}

	std::unordered_map<std::string, std::vector<float>&> inputMap = { { "state-input", m_stateVector } };
	m_QNetwork.getNetwork()->predict(inputMap, m_actionValuePredictionVector);

	int actionValue = a->get(m_outputActionIndex.get());
	int choosenActionIndex = m_pGrid->getClosestCenter(actionValue);

	double targetValue = r + gamma * m_actionValuePredictionVector[choosenActionIndex];
	m_actionValuePredictionVector[choosenActionIndex] = targetValue;

	for (int i = 0; i < m_pStateOutFeatures->m_numFeatures; i++)
	{
		auto item = m_pStateOutFeatures->m_pFeatures[i];
		m_stateVector[item.m_index] = item.m_factor;
	}

	m_QNetwork.getNetwork()->train(inputMap, m_actionValuePredictionVector);

	return 0.0;
}

double CDiscreteSoftmaxDeepPolicy::selectAction(const CState * s, CAction * a)
{
	CSimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(s, m_pStateOutFeatures);

	//TODO: use sparse representation
	for (int i = 0; i < m_pStateOutFeatures->m_numFeatures; i++)
	{
		auto item = m_pStateOutFeatures->m_pFeatures[i];
		m_stateVector[item.m_index] = item.m_factor;
	}

	std::unordered_map<std::string, std::vector<float>&> inputMap = { { "state-input", m_stateVector } };

	m_QNetwork.getNetwork()->predict(inputMap, m_actionValuePredictionVector);

	double sum = 0;
	for (int i = 0; i < m_actionValuePredictionVector.size(); i++)
	{
		m_actionValuePredictionVector[i] = std::exp(m_temperature->get() * m_actionValuePredictionVector[i]);
		sum += m_actionValuePredictionVector[i];
	}
	for (int i = 0; i < m_actionValuePredictionVector.size(); i++)
	{
		m_actionValuePredictionVector[i] /= sum;
	}
	int resultingActionIndex = chooseRandomInteger(m_actionValuePredictionVector);

	double actionValue = m_pGrid->getCenters()[resultingActionIndex];
	a->set(m_outputActionIndex.get(), actionValue);

	return 1.0;
}

double CDiscreteSoftmaxDeepPolicy::updateNetwork(const CState * s, const CAction * a, const CState * s_p, double r)
{
	double gamma = CSimionApp::get()->pSimGod->getGamma();

	CSimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(s, m_pStateOutFeatures);

	//TODO: use sparse representation
	for (int i = 0; i < m_pStateOutFeatures->m_numFeatures; i++)
	{
		auto item = m_pStateOutFeatures->m_pFeatures[i];
		m_stateVector[item.m_index] = item.m_factor;
	}

	std::unordered_map<std::string, std::vector<float>&> inputMap = { { "state-input", m_stateVector } };
	m_QNetwork.getNetwork()->predict(inputMap, m_actionValuePredictionVector);

	int actionValue = a->get(m_outputActionIndex.get());
	int choosenActionIndex = m_pGrid->getClosestCenter(actionValue);

	double targetValue = r + gamma * m_actionValuePredictionVector[choosenActionIndex];
	m_actionValuePredictionVector[choosenActionIndex] = targetValue;

	for (int i = 0; i < m_pStateOutFeatures->m_numFeatures; i++)
	{
		auto item = m_pStateOutFeatures->m_pFeatures[i];
		m_stateVector[item.m_index] = item.m_factor;
	}

	m_QNetwork.getNetwork()->train(inputMap, m_actionValuePredictionVector);

	return 0.0;
}

CDiscreteSoftmaxDeepPolicy::CDiscreteSoftmaxDeepPolicy(CConfigNode * pConfigNode) : CDiscreteDeepPolicy(pConfigNode)
{
	m_temperature = CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "temperature", "Tempreature");
}

#endif