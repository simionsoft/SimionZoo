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

	if (typeid(CSimGod::getGlobalActionFeatureMap().get()) != typeid(CDiscreteActionFeatureMap*))
	{
		throw std::runtime_error("The CDiscreteEpsilonGreedyDeepPolicy requires a CDiscreteActionFeatureMap as the action-feature-map.");
	}

	m_pGrid = ((CSingleDimensionDiscreteActionVariableGrid*)(((CDiscreteActionFeatureMap*)
		CSimGod::getGlobalActionFeatureMap().get())->returnGrid()[m_outputActionIndex.get()]));

	if (m_pGrid->getNumCenters() != m_QNetwork.getNetwork()->getTargetOutput().Shape().TotalSize())
	{
		throw std::runtime_error("Output of the network has not the same size as the discrete action grid has centers/discrete values");
	}
}

double CDiscreteEpsilonGreedyDeepPolicy::selectAction(const CState * s, CAction * a)
{
	double randomValue = getRandomValue();
	int numberOfActions = m_QNetwork.getNetwork()->getTargetOutput().Shape().TotalSize();

	int resultingActionIndex;
	if (randomValue < m_epsilon->get())
	{
		resultingActionIndex = std::rand() % static_cast<int>(numberOfActions + 1);
	}
	else
	{
		CSimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(s, m_pStateOutFeatures);
		int numberOfStateFeatures = ((CDiscreteStateFeatureMap*)CSimionApp::get()->pSimGod->getGlobalStateFeatureMap().get())->returnGrid().size();
		//TODO: Was: CSimionApp::get()->pSimGod->getGlobalStateFeatureMap()-> getTotalNumFeatures()
		//TODO: use sparse representation
		auto stateVector = std::vector<float>(numberOfStateFeatures, 0.0);


		//		for (int i = 0; i < m_pStateOutFeatures->m_numFeatures; i++)
			//		stateVector[m_pStateOutFeatures->m_pFeatures[i].m_index] = m_pStateOutFeatures->m_pFeatures[i].m_factor;

		for (int i = 0; i < numberOfStateFeatures; i++)
			stateVector[i] = ((CDiscreteStateFeatureMap*)CSimionApp::get()->pSimGod->getGlobalStateFeatureMap().get())->returnGrid()[i]->getVarValue(s, nullptr);

		auto prediction = std::vector<float>(numberOfActions);
		std::unordered_map<std::string, std::vector<float>&> inputMap = { { "state-input", stateVector } };
		m_QNetwork.getNetwork()->predict(inputMap, prediction, 1);
		resultingActionIndex = std::distance(prediction.begin(), std::max_element(prediction.begin(), prediction.end()));
	}

	//TODO: check if this gets the correct action index
	//TODO: check if this gets the value from this index
	double actionValue = m_pGrid->getCenters()[resultingActionIndex];
	a->set(m_outputActionIndex.get(), actionValue);

	return 0.0;
}

double CDiscreteEpsilonGreedyDeepPolicy::updateNetwork(const CState * s, const CAction * a, const CState * s_p, double r)
{
	int numberOfActions = m_QNetwork.getNetwork()->getTargetOutput().Shape().TotalSize();
	double gamma = CSimionApp::get()->pSimGod->getGamma();

	CSimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(s, m_pStateOutFeatures);

	//TODO: use sparse representation
	int numberOfStateFeatures = ((CDiscreteStateFeatureMap*)CSimionApp::get()->pSimGod->getGlobalStateFeatureMap().get())->returnGrid().size();
	auto stateVector = std::vector<float>(numberOfStateFeatures, 0.0);
	for (int i = 0; i < numberOfStateFeatures; i++)
		stateVector[i] = ((CDiscreteStateFeatureMap*)CSimionApp::get()->pSimGod->getGlobalStateFeatureMap().get())->returnGrid()[i]->getVarValue(s_p, nullptr);

	auto prediction = std::vector<float>(numberOfActions);
	std::unordered_map<std::string, std::vector<float>&> inputMap = { { "state-input", stateVector } };
	m_QNetwork.getNetwork()->predict(inputMap, prediction, 1);

	int actionValue = a->get(m_outputActionIndex.get());
	int choosenActionIndex = m_pGrid->getClosestCenter(actionValue);

	double targetValue = r + gamma * prediction[choosenActionIndex];
	prediction[choosenActionIndex] = targetValue;

	for (int i = 0; i < numberOfStateFeatures; i++)
		stateVector[i] = ((CDiscreteStateFeatureMap*)CSimionApp::get()->pSimGod->getGlobalStateFeatureMap().get())->returnGrid()[i]->getVarValue(s, nullptr);
	//std::unordered_map<std::string, std::vector<float>&> inputMap = { { "state-input", stateVector } };

	m_QNetwork.getNetwork()->train(stateVector, prediction, 1);

	return 0.0;
}
