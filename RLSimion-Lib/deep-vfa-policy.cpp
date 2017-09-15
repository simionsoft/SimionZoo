#include "deep-vfa-policy.h"
#include "parameters.h"
#include "SimGod.h"
#include "app.h"
#include "noise.h"
#include "featuremap.h"
#include "features.h"
#include "single-dimension-discrete-grid.h"

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
		{ "Discrete-Deep-Policy",CHOICE_ELEMENT_NEW<CDiscreteEpsilonGreedyDeepPolicy> }
	});
}

CDiscreteEpsilonGreedyDeepPolicy::CDiscreteEpsilonGreedyDeepPolicy(CConfigNode * pConfigNode) : CDeepPolicy(pConfigNode)
{
	m_pStateOutFeatures = new CFeatureList("State");
	m_epsilon = CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "epsilon", "Epsilon");

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

	//TODO: Was: CSimionApp::get()->pSimGod->getGlobalStateFeatureMap()-> getTotalNumFeatures()
	m_numberOfStateFeatures = CSimGod::getGlobalStateFeatureMap()->getTotalNumFeatures();

	if (m_numberOfStateFeatures == 0)
		CLogger::logMessage(MessageType::Error, "Invalid State-Map chosen (it will return 0 features at most, but at least 1 has to be returned)");

	//((CDiscreteStateFeatureMap*)CSimionApp::get()->pSimGod->getGlobalStateFeatureMap().get())->returnGrid().size();
	m_numberOfActions = m_QNetwork.getNetwork()->getTargetOutput().Shape().TotalSize();
	m_stateVector = std::vector<float>(m_numberOfStateFeatures, 0.0);
	m_actionValuePredictionVector = std::vector<float>(m_numberOfActions);
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
		//((CDiscreteStateFeatureMap*)CSimGod::getGlobalStateFeatureMap().get())->returnGrid()[i]->getVarValue(s, nullptr);
		CNTK::ValuePtr stateSequence = CNTK::Value::CreateSequence({ (size_t)m_numberOfStateFeatures }, m_stateVector, CNTK::DeviceDescriptor::CPUDevice());

		std::unordered_map<std::string, CNTK::ValuePtr> inputMap = { { "state-input", stateSequence } };
		m_QNetwork.getNetwork()->predict(inputMap, m_actionValuePredictionVector);
		resultingActionIndex = std::distance(m_actionValuePredictionVector.begin(),
			std::max_element(m_actionValuePredictionVector.begin(), m_actionValuePredictionVector.end()));
	}

	//TODO: check if this gets the correct action index
	//TODO: check if this gets the value from this index
	double actionValue = m_pGrid->getCenters()[resultingActionIndex];
	a->set(m_outputActionIndex.get(), actionValue);

	return 1.0;
}

double CDiscreteEpsilonGreedyDeepPolicy::updateNetwork(const CState * s, const CAction * a, const CState * s_p, double r)
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
	m_QNetwork.getNetwork()->predict(inputMap, m_actionValuePredictionVector, 1);

	int actionValue = a->get(m_outputActionIndex.get());
	int choosenActionIndex = m_pGrid->getClosestCenter(actionValue);

	double targetValue = r + gamma * m_actionValuePredictionVector[choosenActionIndex];
	m_actionValuePredictionVector[choosenActionIndex] = targetValue;

	for (int i = 0; i < m_pStateOutFeatures->m_numFeatures; i++)
	{
		auto item = m_pStateOutFeatures->m_pFeatures[i];
		m_stateVector[item.m_index] = item.m_factor;
	}
	//std::unordered_map<std::string, std::vector<float>&> inputMap = { { "state-input", stateVector } };

	m_QNetwork.getNetwork()->train(m_stateVector, m_actionValuePredictionVector, 1);

	return 0.0;
}
