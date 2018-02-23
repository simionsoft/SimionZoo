#ifdef _WIN64

#include "DQN.h"
#include "SimGod.h"
#include "worlds\world.h"
#include "app.h"
#include "featuremap.h"
#include "features.h"
#include "noise.h"
#include "deep-vfa-policy.h"
#include "parameters-numeric.h"
#include "../tools/CNTKWrapper/CNTKWrapper.h"
#include "../tools/CNTKWrapper/Network.h"

CDQN::~CDQN()
{
	delete m_pStateOutFeatures;
	delete[] m_pMinibatchExperienceTuples;
	delete[] m_pMinibatchChosenActionTargetValues;
	delete[] m_pMinibatchChosenActionIndex;
	CNTKWrapper::Close();
}

CDQN::CDQN(CConfigNode* pConfigNode)
{
	CNTKWrapper::Init();
	m_policy = CHILD_OBJECT_FACTORY<CDiscreteDeepPolicy>(pConfigNode, "Policy", "The policy");
	m_QNetwork = NEURAL_NETWORK_PROBLEM_DESCRIPTION(pConfigNode, "neural-network", "Neural Network Architecture");
	m_outputActionIndex = ACTION_VARIABLE(pConfigNode, "Output-Action", "The output action variable");
	m_experienceReplay = CHILD_OBJECT<CExperienceReplay>(pConfigNode, "experience-replay", "Experience replay", false);

	m_pStateOutFeatures = new CFeatureList("state-input");

	m_numberOfStateFeatures = CSimGod::getGlobalStateFeatureMap()->getTotalNumFeatures();
	if (m_numberOfStateFeatures == 0)
		CLogger::logMessage(MessageType::Error, "Invalid State-Map chosen (it will return 0 features at most, but at least 1 has to be returned)");

	if (dynamic_cast<CDiscreteActionFeatureMap*>(CSimGod::getGlobalActionFeatureMap().get()) == nullptr)
		CLogger::logMessage(MessageType::Error, "The CDiscreteEpsilonGreedyDeepPolicy requires a CDiscreteActionFeatureMap as the action-feature-map.");

	m_pGrid = ((CSingleDimensionDiscreteActionVariableGrid*)(((CDiscreteActionFeatureMap*)CSimGod::getGlobalActionFeatureMap().get())->returnGrid()[m_outputActionIndex.get()]));

	if (m_pGrid->getNumCenters() != m_QNetwork.getNetwork()->getTargetOutput().Shape().TotalSize())
		CLogger::logMessage(MessageType::Error, "Output of the network has not the same size as the discrete action grid has centers/discrete values");

	m_numberOfActions = m_QNetwork.getNetwork()->getTargetOutput().Shape().TotalSize();
	m_stateVector = std::vector<double>(m_numberOfStateFeatures, 0.0);
	m_actionValuePredictionVector = std::vector<double>(m_numberOfActions);

	m_minibatchStateVector = std::vector<double>(m_numberOfStateFeatures * m_experienceReplay->getMaxUpdateBatchSize(), 0.0);
	m_minibatchActionValuePredictionVector = std::vector<double>(m_numberOfActions * m_experienceReplay->getMaxUpdateBatchSize(), 0.0);

	m_pMinibatchExperienceTuples = new CExperienceTuple*[m_experienceReplay->getMaxUpdateBatchSize()];
	m_pMinibatchChosenActionTargetValues = new double[m_experienceReplay->getMaxUpdateBatchSize()];
	m_pMinibatchChosenActionIndex = new int[m_experienceReplay->getMaxUpdateBatchSize()];


}

CNetwork* CDQN::getPredictionNetwork()
{
	if (CSimionApp::get()->pSimGod->getTargetFunctionUpdateFreq())
	{
		if (m_pPredictionNetwork == nullptr)
			m_pPredictionNetwork = m_QNetwork.getNetwork()->cloneNonTrainable();
		return m_pPredictionNetwork;
	}
	else
		return m_QNetwork.getNetwork();
}

double CDQN::selectAction(const CState * s, CAction * a)
{
	CSimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(s, m_pStateOutFeatures);

	//TODO: use sparse representation
	for (size_t i = 0; i < m_pStateOutFeatures->m_numFeatures; i++)
	{
		auto item = m_pStateOutFeatures->m_pFeatures[i];
		m_stateVector[item.m_index] = item.m_factor;
	}

	std::unordered_map<std::string, std::vector<double>&> inputMap = { { "state-input", m_stateVector } };

	getPredictionNetwork()->predict(inputMap, m_actionValuePredictionVector);

	size_t resultingActionIndex = m_policy->selectAction(m_actionValuePredictionVector);

	double actionValue = m_pGrid->getCenters()[resultingActionIndex];
	a->set(m_outputActionIndex.get(), actionValue);

	return 1.0;
}

double CDQN::update(const CState * s, const CAction * a, const CState * s_p, double r, double behaviorProb)
{
	double gamma = CSimionApp::get()->pSimGod->getGamma();

	m_experienceReplay->addTuple(s, a, s_p, r, 1.0);

	//don't update anything if the experience replay buffer does not contain enough elements for at least one minibatch
	if (m_experienceReplay->getUpdateBatchSize() != m_experienceReplay->getMaxUpdateBatchSize())
		return 0.0;

	for (int i = 0; i < m_experienceReplay->getMaxUpdateBatchSize(); i++)
	{
		m_pMinibatchExperienceTuples[i] = m_experienceReplay->getRandomTupleFromBuffer();

		//get Q(s_p) for entire minibatch
		CSimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(m_pMinibatchExperienceTuples[i]->s_p, m_pStateOutFeatures);
		for (int n = 0; n < m_pStateOutFeatures->m_numFeatures; n++)
			m_minibatchStateVector[m_pStateOutFeatures->m_pFeatures[n].m_index + i*m_numberOfStateFeatures] = m_pStateOutFeatures->m_pFeatures[n].m_factor;

		m_pMinibatchChosenActionIndex[i] = m_pGrid->getClosestCenter(m_pMinibatchExperienceTuples[i]->a->get(m_outputActionIndex.get()));
	}

	std::unordered_map<std::string, std::vector<double>&> inputMap = { { "state-input", m_minibatchStateVector } };
	getPredictionNetwork()->predict(inputMap, m_minibatchActionValuePredictionVector);


	//create vector of target values for the entire minibatch
	for (int i = 0; i < m_experienceReplay->getMaxUpdateBatchSize(); i++)
		m_pMinibatchChosenActionTargetValues[i] = m_pMinibatchExperienceTuples[i]->r + gamma *m_minibatchActionValuePredictionVector[
			i*m_numberOfActions + m_pMinibatchChosenActionIndex[i]];


	//get Q(s) for entire minibatch
	for (int i = 0; i < m_experienceReplay->getMaxUpdateBatchSize(); i++)
	{
		CSimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(m_pMinibatchExperienceTuples[i]->s, m_pStateOutFeatures);
		for (int n = 0; n < m_pStateOutFeatures->m_numFeatures; n++)
			m_minibatchStateVector[m_pStateOutFeatures->m_pFeatures[n].m_index + i*m_numberOfStateFeatures] = m_pStateOutFeatures->m_pFeatures[n].m_factor;
	}
	getPredictionNetwork()->predict(inputMap, m_minibatchActionValuePredictionVector);

	for (int i = 0; i < m_experienceReplay->getMaxUpdateBatchSize(); i++)
		m_minibatchActionValuePredictionVector[m_pMinibatchChosenActionIndex[i] + i*m_numberOfActions] = m_pMinibatchChosenActionTargetValues[i];

	//update the network finally
	m_QNetwork.getNetwork()->train(inputMap, m_minibatchActionValuePredictionVector);

	if (CSimionApp::get()->pSimGod->getTargetFunctionUpdateFreq())
		if (CSimionApp::get()->pExperiment->getExperimentStep() % CSimionApp::get()->pSimGod->getTargetFunctionUpdateFreq() == 0)
		{
			m_pPredictionNetwork = m_QNetwork.getNetwork()->cloneNonTrainable();
		}
	return 0.0; //TODO: what should we return?
}

#endif