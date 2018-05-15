#ifdef _WIN64
#include "async-deep-simion.h"	

#include "SimGod.h"
#include "worlds\world.h"
#include "app.h"
#include "featuremap.h"
#include "features.h"
#include "noise.h"
#include "deep-vfa-policy.h"
#include "parameters-numeric.h"
#include "q-learners.h"
#include "experience-replay.h"
#include "critic.h"

#include "../tools/CNTKWrapper/CNTKWrapper.h"

#include <algorithm>
/*
AsyncQLearning::AsyncQLearning(ConfigNode* pConfigNode)
{
	CNTKWrapperLoader::Load();
	m_policy = CHILD_OBJECT_FACTORY<DiscreteDeepPolicy>(pConfigNode, "Policy", "The policy");
	m_predictionQNetwork = NN_DEFINITION(pConfigNode, "neural-network", "Neural Network Architecture");
	m_outputAction = ACTION_VARIABLE(pConfigNode, "Output-Action", "The output action variable");
	m_experienceReplay = CHILD_OBJECT<ExperienceReplay>(pConfigNode, "experience-replay", "Experience replay", false);

	m_iAsyncUpdate = INT_PARAM(pConfigNode, "i-async-update", "Steps before the network's weights are updated by the optimizer with the accumulated DeltaTheta", 1);
	m_iTarget = INT_PARAM(pConfigNode, "i-target", "Steps before the target network's weights are updated", 1);

	m_pStateOutFeatures = new FeatureList("state-input");

	m_numberOfStateVars = SimGod::getGlobalStateFeatureMap()->getTotalNumFeatures();
	if (m_numberOfStateVars == 0)
		Logger::logMessage(MessageType::Error, "Invalid State-Map chosen (it will return 0 features at most, but at least 1 has to be returned)");

	if (dynamic_cast<DiscreteActionFeatureMap*>(SimGod::getGlobalActionFeatureMap().get()) == nullptr)
		Logger::logMessage(MessageType::Error, "The CDiscreteEpsilonGreedyDeepPolicy requires a CDiscreteActionFeatureMap as the action-feature-map.");

	m_pGrid = ((SingleDimensionDiscreteActionVariableGrid*)
		(((DiscreteActionFeatureMap*)SimGod::getGlobalActionFeatureMap().get())->returnGrid()[m_outputAction.get()]));

	if (m_pGrid->getNumCenters() != m_predictionQNetwork.getNetwork()->getTotalSize());
		Logger::logMessage(MessageType::Error, "Output of the network has not the same size as the discrete action grid has centers/discrete values");

	m_numberOfActions = m_predictionQNetwork.getNetwork()->getTargetOutput().Shape().TotalSize();
	m_stateVector = std::vector<float>(m_numberOfStateVars, 0.0);
	m_actionValuePredictionVector = std::vector<float>(m_numberOfActions);

	m_minibatch_s = std::vector<float>(m_numberOfStateVars * m_experienceReplay->getMaxUpdateBatchSize(), 0.0);
	m_minibatch_Q_s = std::vector<float>(m_numberOfActions * m_experienceReplay->getMaxUpdateBatchSize(), 0.0);

	m_pMinibatchExperienceTuples = new ExperienceTuple*[m_experienceReplay->getMaxUpdateBatchSize()];
	m_pMinibatchChosenActionTargetValues = new double[m_experienceReplay->getMaxUpdateBatchSize()];
	m_pMinibatchActionId = new int[m_experienceReplay->getMaxUpdateBatchSize()];

	CNTKWrapperLoader::Load();
	m_pTargetQNetwork = m_predictionQNetwork.getNetwork()->clone();
}

AsyncQLearning::~AsyncQLearning()
{
	CNTKWrapperLoader::UnLoad();
}

double AsyncQLearning::selectAction(const State * s, Action * a)
{
	SimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(s, m_pStateOutFeatures);

	//TODO: use sparse representation
	for (int i = 0; i < m_pStateOutFeatures->m_numFeatures; i++)
	{
		auto item = m_pStateOutFeatures->m_pFeatures[i];
		m_stateVector[item.m_index] = item.m_factor;
	}

	std::unordered_map<std::string, std::vector<double>&> inputMap =
		{ { "state-input", m_stateVector } };

	m_predictionQNetwork.getNetwork()->predict(inputMap, m_actionValuePredictionVector);

	int resultingActionIndex = m_policy->selectAction(m_actionValuePredictionVector);

	double actionValue = m_pGrid->getCenters()[resultingActionIndex];
	a->set(m_outputAction.get(), actionValue);

	return 1.0;
}

double AsyncQLearning::update(const State * s, const Action * a, const State * s_p, double r, double behaviorProb)
{
	double gamma = SimionApp::get()->pSimGod->getGamma();

	m_experienceReplay->addTuple(s, a, s_p, r, 1.0);

	//don't update anything if the experience replay buffer does not contain enough elements for at least one minibatch
	if (m_experienceReplay->getUpdateBatchSize() != m_experienceReplay->getMaxUpdateBatchSize())
		return 0.0;

	for (int i = 0; i < m_experienceReplay->getMaxUpdateBatchSize(); i++)
	{
		m_pMinibatchExperienceTuples[i] = m_experienceReplay->getRandomTupleFromBuffer();

		//get Q(s_p) for entire minibatch
		SimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(m_pMinibatchExperienceTuples[i]->s_p, m_pStateOutFeatures);
		for (int n = 0; n < m_pStateOutFeatures->m_numFeatures; n++)
			m_minibatch_s[m_pStateOutFeatures->m_pFeatures[n].m_index + i*m_numberOfStateVars] = m_pStateOutFeatures->m_pFeatures[n].m_factor;

		m_pMinibatchActionId[i] = m_pGrid->getClosestValue(m_pMinibatchExperienceTuples[i]->a->get(m_outputAction.get()));
	}

	std::unordered_map<std::string, std::vector<double>&> inputMap
		= { { "state-input", m_minibatch_s } };
	m_pTargetQNetwork->predict(inputMap, m_minibatch_Q_s);


	//create vector of target values for the entire minibatch
	for (int i = 0; i < m_experienceReplay->getMaxUpdateBatchSize(); i++)
	{
		int argmaxQ = std::distance(m_minibatch_Q_s.begin() + i*m_numberOfActions,
			std::max_element(m_minibatch_Q_s.begin() + i*m_numberOfActions, m_minibatch_Q_s.begin() + (i + 1)*m_numberOfActions));
		m_pMinibatchChosenActionTargetValues[i] = m_pMinibatchExperienceTuples[i]->r + gamma * m_minibatch_Q_s[i*m_numberOfActions + argmaxQ];
	}


	//get Q(s) for entire minibatch
	for (int i = 0; i < m_experienceReplay->getMaxUpdateBatchSize(); i++)
	{
		SimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(m_pMinibatchExperienceTuples[i]->s, m_pStateOutFeatures);
		for (int n = 0; n < m_pStateOutFeatures->m_numFeatures; n++)
			m_minibatch_s[m_pStateOutFeatures->m_pFeatures[n].m_index + i*m_numberOfStateVars] = m_pStateOutFeatures->m_pFeatures[n].m_factor;
	}
	m_predictionQNetwork.getNetwork()->predict(inputMap, m_minibatch_Q_s);

	for (int i = 0; i < m_experienceReplay->getMaxUpdateBatchSize(); i++)
		m_minibatch_Q_s[m_pMinibatchActionId[i] + i*m_numberOfActions] = m_pMinibatchChosenActionTargetValues[i];

	//calculate the weights changes of the network finally
	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> gradients;
	std::static_pointer_cast<Network>(m_predictionQNetwork.getNetwork())
		->gradients(inputMap, m_minibatch_Q_s, gradients);

	//aggregate the weights
	for each (auto tuple in m_aggregatedGradients)
	{
		auto leftInput = CNTK::InputVariable(tuple.first.Shape(), CNTK::DataType::Float);
		auto rightInput = CNTK::InputVariable(tuple.first.Shape(), CNTK::DataType::Float);
		auto add = CNTK::Plus(leftInput, rightInput);
		std::unordered_map<CNTK::Variable, CNTK::ValuePtr> inputMap = { { leftInput , tuple.second },{ rightInput,gradients[tuple.first] } };
		std::unordered_map<CNTK::Variable, CNTK::ValuePtr> outputMap = { { add , tuple.second } };
		add->Evaluate(inputMap, outputMap);
	}

	if (SimionApp::get()->pExperiment->getExperimentStep() % m_iTarget.get() == 0)
	{
		//update the target network
		for (int i = 0; i < m_pTargetQNetwork->getNetworkFunctionPtr()->Parameters().size(); i++)
		{
			m_pTargetQNetwork->getNetworkFunctionPtr()->Parameters()[i].SetValue(m_predictionQNetwork.getNetwork()->getNetworkFunctionPtr()->Parameters()[i].Value());
		}
	}

	if (SimionApp::get()->pExperiment->getStep() % m_iAsyncUpdate.get() == 0 || SimionApp::get()->pExperiment->isLastStep())
	{
		//update prediction network using the aggregated gradients inside the buffer
		m_predictionQNetwork.getNetwork()->getTrainer()->ParameterLearners()[0]->Update(m_aggregatedGradientValues, m_aggregatedGradientCount);

		//reset the gradient buffer
		for each (auto view in m_aggregatedGradientValues)
		{
			view.second->SetValue(0.0);
		}
	}
}
*/

#endif