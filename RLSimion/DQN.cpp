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
#include "parameters.h"
#include "../tools/CNTKWrapper/CNTKWrapper.h"
#include <algorithm>
#include "deep-vfa-policy.h"

DQN::~DQN()
{
	if (m_pMinibatchChosenActionTargetValues)
		delete[] m_pMinibatchChosenActionTargetValues;
	if (m_pMinibatchChosenActionIndex)
		delete[] m_pMinibatchChosenActionIndex;

	//We need to manually call the NN_DEFINITION destructor
	m_pNNDefinition.destroy();
	m_pTargetQNetwork->destroy();
	m_pPredictionQNetwork->destroy();
	CNTKWrapperLoader::UnLoad();
}

DQN::DQN(ConfigNode* pConfigNode)
{
	CNTKWrapperLoader::Load();
	m_policy = CHILD_OBJECT_FACTORY<DiscreteDeepPolicy>(pConfigNode, "Policy", "The policy");
	m_pNNDefinition = NN_DEFINITION(pConfigNode, "neural-network", "Neural Network Architecture");

	m_outputActionIndex = ACTION_VARIABLE(pConfigNode, "Output-Action", "The output action variable");

	m_numberOfStateVars = SimionApp::get()->pWorld->getDynamicModel()->getStateDescriptor().size();
	if (m_numberOfStateVars == 0)
		Logger::logMessage(MessageType::Error, "Invalid State-Map chosen (it will return 0 features at most, but at least 1 has to be returned)");

	if (dynamic_cast<DiscreteActionFeatureMap*>(SimGod::getGlobalActionFeatureMap().get()) == nullptr)
		Logger::logMessage(MessageType::Error, "The DiscreteEpsilonGreedyDeepPolicy requires a DiscreteActionFeatureMap as the action-feature-map.");
	
	m_pGrid = ((SingleDimensionDiscreteActionVariableGrid*)(((DiscreteActionFeatureMap*)SimGod::getGlobalActionFeatureMap().get())->returnGrid()[m_outputActionIndex.get()]));
}

void DQN::deferredLoadStep()
{
	//heavy-weight loading and stuff that relies on the SimGod
	m_pTargetQNetwork = m_pNNDefinition.buildNetwork();
	m_pPredictionQNetwork = m_pTargetQNetwork->cloneNonTrainable();

	m_numberOfActions = m_pTargetQNetwork->getTotalSize();
	m_minibatchSize = SimionApp::get()->pSimGod->getExperienceReplayUpdateSize();

	if (m_pGrid->getNumCenters() != m_numberOfActions)
		Logger::logMessage(MessageType::Error, "Output of the network has not the same size as the discrete action grid has centers/discrete values");

	m_stateVector = std::vector<double>(m_numberOfStateVars, 0.0);
	m_actionValuePredictionVector = std::vector<double>(m_numberOfActions);

	m_minibatchStateVector = std::vector<double>(m_numberOfStateVars * m_minibatchSize, 0.0);
	m_minibatchActionValuePredictionVector = std::vector<double>(m_numberOfActions * m_minibatchSize, 0.0);

	m_pMinibatchChosenActionTargetValues = new double[m_minibatchSize];
	m_pMinibatchChosenActionIndex = new size_t[m_minibatchSize];
}

INetwork* DQN::getPredictionNetwork()
{
	return m_pPredictionQNetwork;
}

double DQN::selectAction(const State * s, Action * a)
{
	//SimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(s, m_pStateOutFeatures);

	////TODO: use sparse representation
	//for (size_t i = 0; i < m_pStateOutFeatures->m_numFeatures; i++)
	//{
	//	auto item = m_pStateOutFeatures->m_pFeatures[i];
	//	m_stateVector[item.m_index] = item.m_factor;
	//}
	for (size_t i = 0; i < s->getNumVars(); i++)
	{
		m_stateVector[i] = s->get((int) i);
	}

	std::unordered_map<std::string, std::vector<double>&> inputMap =
		{ { "state-input", m_stateVector } };

	getPredictionNetwork()->predict(inputMap, m_actionValuePredictionVector);

	size_t resultingActionIndex = m_policy->selectAction(m_actionValuePredictionVector);

	double actionValue = m_pGrid->getCenters()[resultingActionIndex];
	a->set(m_outputActionIndex.get(), actionValue);

	return 1.0;
}

double DQN::update(const State * s, const Action * a, const State * s_p, double r, double behaviorProb)
{
	double gamma = SimionApp::get()->pSimGod->getGamma();

	size_t numInputVariables = s->getNumVars(); //TODO: FIX THIS
	//get Q(s_p) for the current tuple
	for (size_t i = 0; i < numInputVariables; i++)
	{
		m_minibatchStateVector[m_minibatchTuples*numInputVariables + i] = s_p->get((int) i);
	}
	//store the index of the action taken
	m_pMinibatchChosenActionIndex[m_minibatchTuples] =
		m_pGrid->getClosestCenter(a->get(m_outputActionIndex.get()));

	//estimate Q(s_p)
	std::unordered_map<std::string, std::vector<double>&> inputMap = 
		{ { "state-input", m_minibatchStateVector } };
	getPredictionNetwork()->predict(inputMap, m_minibatchActionValuePredictionVector);

	//calculate and store the target for the current tuple
	size_t argmaxQ = 
		std::distance(m_minibatchActionValuePredictionVector.begin() + m_minibatchTuples * m_numberOfActions,
		std::max_element(m_minibatchActionValuePredictionVector.begin() 
			+ m_minibatchTuples * m_numberOfActions, m_minibatchActionValuePredictionVector.begin() 
			+ (m_minibatchTuples + 1)*m_numberOfActions));
	m_pMinibatchChosenActionTargetValues[m_minibatchTuples] =
		r + gamma * m_minibatchActionValuePredictionVector[m_minibatchTuples*m_numberOfActions + argmaxQ];

	//estimate Q(s,a)
	for (size_t i = 0; i < s->getNumVars(); i++)
	{
		m_minibatchStateVector[m_minibatchTuples*s->getNumVars() + i] = s->get((int) i);
	}
	getPredictionNetwork()->predict(inputMap, m_minibatchActionValuePredictionVector);

	m_minibatchActionValuePredictionVector
		[m_pMinibatchChosenActionIndex[m_minibatchTuples] + m_minibatchTuples*m_numberOfActions] = 
			m_pMinibatchChosenActionTargetValues[m_minibatchTuples];

	m_minibatchTuples++;

	//We only train the network in direct-experience updates to simplify mini-batching
	SimGod* pSimGod = SimionApp::get()->pSimGod.ptr();

	//update the network finally
	if (!pSimGod->bReplayingExperience() && m_minibatchTuples>1)
	{
		m_pTargetQNetwork->train(inputMap, m_minibatchActionValuePredictionVector);
		m_minibatchTuples = 0;
	}

	//update the prediction network
	if (pSimGod->bUpdateFrozenWeightsNow())
	{
		if (m_pPredictionQNetwork)
			m_pPredictionQNetwork->destroy();
		m_pPredictionQNetwork = m_pTargetQNetwork->cloneNonTrainable();
	}
	return 1.0; //TODO: Estimate the TD-error??
}
/*
INetwork* DoubleDQN::getPredictionNetwork()
{
	if (SimionApp::get()->pSimGod->getTargetFunctionUpdateFreq())
	{
		if (m_pPredictionQNetwork == nullptr)
			m_pPredictionQNetwork = m_pTargetQNetwork->cloneNonTrainable();
		return m_pPredictionQNetwork;
	}
	else
		Logger::logMessage(MessageType::Error, "Freeze-Target-Function is disabled, but the Double-DQN algorithm relies on this feature.");
}

double DoubleDQN::update(const State * s, const Action * a, const State * s_p, double r, double behaviorProb)
{
	double gamma = SimionApp::get()->pSimGod->getGamma();

	m_experienceReplay->addTuple(s, a, s_p, r, 1.0);

	//don't update anything if the experience replay buffer does not contain enough elements for at least one minibatch
	if (m_experienceReplay->getUpdateBatchSize() != m_experienceReplay->getUpdateBatchSize())
		return 0.0;

	for (int i = 0; i < m_experienceReplay->getUpdateBatchSize(); i++)
	{
		m_pMinibatchExperienceTuples[i] = m_experienceReplay->getRandomTupleFromBuffer();

		//get Q(s_p) for entire minibatch
		SimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(m_pMinibatchExperienceTuples[i]->s_p, m_pStateOutFeatures);
		for (int n = 0; n < m_pStateOutFeatures->m_numFeatures; n++)
			m_minibatchStateVector[m_pStateOutFeatures->m_pFeatures[n].m_index + i * m_numberOfStateVars] = m_pStateOutFeatures->m_pFeatures[n].m_factor;

		m_pMinibatchChosenActionIndex[i] = m_pGrid->getClosestCenter(m_pMinibatchExperienceTuples[i]->a->get(m_outputActionIndex.get()));
	}

	std::unordered_map<std::string, std::vector<double>&> inputMap =
		{ { "state-input", m_minibatchStateVector } };
	//this is the ONLY difference compared to normal DQN algorithm
	m_pTargetQNetwork->predict(inputMap, m_minibatchActionValuePredictionVector);


	//create vector of target values for the entire minibatch
	for (int i = 0; i < m_experienceReplay->getUpdateBatchSize(); i++)
	{
		int argmaxQ = std::distance(m_minibatchActionValuePredictionVector.begin() + i * m_numberOfActions,
			std::max_element(m_minibatchActionValuePredictionVector.begin() + i * m_numberOfActions, m_minibatchActionValuePredictionVector.begin() + (i + 1)*m_numberOfActions));
		m_pMinibatchChosenActionTargetValues[i] = m_pMinibatchExperienceTuples[i]->r + gamma * m_minibatchActionValuePredictionVector[i*m_numberOfActions + argmaxQ];
	}


	//get Q(s) for entire minibatch
	for (int i = 0; i < m_experienceReplay->getUpdateBatchSize(); i++)
	{
		SimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(m_pMinibatchExperienceTuples[i]->s, m_pStateOutFeatures);
		for (int n = 0; n < m_pStateOutFeatures->m_numFeatures; n++)
			m_minibatchStateVector[m_pStateOutFeatures->m_pFeatures[n].m_index + i * m_numberOfStateVars] = m_pStateOutFeatures->m_pFeatures[n].m_factor;
	}
	getPredictionNetwork()->predict(inputMap, m_minibatchActionValuePredictionVector);

	for (int i = 0; i < m_experienceReplay->getUpdateBatchSize(); i++)
		m_minibatchActionValuePredictionVector[m_pMinibatchChosenActionIndex[i] + i * m_numberOfActions] = m_pMinibatchChosenActionTargetValues[i];

	//update the network finally
	m_pTargetQNetwork->train(inputMap, m_minibatchActionValuePredictionVector);

	if (SimionApp::get()->pSimGod->getTargetFunctionUpdateFreq())
	{
		if (SimionApp::get()->pExperiment->getExperimentStep() % SimionApp::get()->pSimGod->getTargetFunctionUpdateFreq() == 0)
		{
			delete m_pPredictionQNetwork;
			m_pPredictionQNetwork = m_pTargetQNetwork->cloneNonTrainable();
		}
	}

	return 0.0;
}

#include "../tools/CNTKWrapper/CNTKLibrary.h"

//quick fix to make it compile. Will fix it later
std::unordered_map<CNTK::Parameter, CNTK::FunctionPtr> m_weightTransitions;


void DDPG::buildModelsParametersTransitionGraph()
{
	float tau = (float)m_tau.get();

	auto scale = CNTK::Constant::Scalar(CNTK::DataType::Float, tau, CNTK::DeviceDescriptor::CPUDevice());
	auto anitScale = CNTK::Constant::Scalar(CNTK::DataType::Float, 1.0f - tau, CNTK::DeviceDescriptor::CPUDevice());

	for (int modelIndex = 0; modelIndex <= 1; modelIndex++)
	{
		auto predictionModel = (modelIndex == 0 ? m_predictionPolicyNetwork : m_predictionQNetwork).getNetwork()->getNetworkFunctionPtr();
		auto targetModel = (modelIndex == 0 ? m_pTargetPolicyNetwork : m_pTargetQNetwork)->getNetworkFunctionPtr();

		for (int i = 0; i < predictionModel->Parameters().size(); i++)
		{
			auto targetParam = targetModel->Parameters()[i];
			auto predictionParam = predictionModel->Parameters()[i];

			auto weightTransition = CNTK::Plus(CNTK::ElementTimes(scale, predictionParam), CNTK::ElementTimes(anitScale, targetParam));
			m_weightTransitions[targetParam] = weightTransition;
		}
	}
}

//TODO: Extract in NNCreator project with wrapper functions, so that one does not have to call CNTK methods in RLSimion-Lib
void DDPG::performModelsParametersTransition()
{
	for (int modelIndex = 0; modelIndex <= 1; modelIndex++)
	{
		auto predictionModel = (modelIndex == 0 ? m_predictionPolicyNetwork : m_predictionQNetwork).getNetwork()->getNetworkFunctionPtr();
		auto targetModel = (modelIndex == 0 ? m_pTargetPolicyNetwork : m_pTargetQNetwork)->getNetworkFunctionPtr();

		for (int i = 0; i < predictionModel->Parameters().size(); i++)
		{
			auto targetParam = targetModel->Parameters()[i];
			auto weightTransition = m_weightTransitions[targetParam];

			CNTK::ValuePtr weightTransitionOutputValue;
			std::unordered_map<CNTK::Variable, CNTK::ValuePtr> outputs = { { weightTransition->Output(), weightTransitionOutputValue } };
			weightTransition->Evaluate({}, outputs, CNTK::DeviceDescriptor::CPUDevice());
			weightTransitionOutputValue = outputs[weightTransition->Output()];

			targetParam.SetValue(weightTransitionOutputValue->Data());
		}
	}
}

DDPG::~DDPG()
{
}

DDPG::DDPG(ConfigNode * pConfigNode)
{
	m_experienceReplay = CHILD_OBJECT<ExperienceReplay>(pConfigNode, "experience-replay", "Experience Replay", false);
	m_predictionQNetwork = NN_DEFINITION(pConfigNode, "q-neural-network", "Neural Network Architecture of Critic (Q)");
	m_predictionPolicyNetwork = NN_DEFINITION(pConfigNode, "policy-neural-network", "Neural Network Architecture of Actor (mu)");
	m_policyNoise = CHILD_OBJECT_FACTORY<Noise>(pConfigNode, "noise", "Policy Noise", false);
	m_tau = DOUBLE_PARAM(pConfigNode, "tau", "tau", 0.001);
	m_outputActionIndex = ACTION_VARIABLE(pConfigNode, "Output-Action", "The output action variable");

	m_pStateFeatureList = new FeatureList("state-input");

	m_numberOfStateVars = SimGod::getGlobalStateFeatureMap()->getTotalNumFeatures();
	if (m_numberOfStateVars == 0)
		Logger::logMessage(MessageType::Error, "Invalid State-Map chosen (it will return 0 features at most, but at least 1 has to be returned)");

	m_pMinibatchExperienceTuples = new ExperienceTuple*[m_experienceReplay->getUpdateBatchSize()];

	m_stateVector = std::vector<double>(m_numberOfStateVars);
	m_actionPredictionVector = std::vector<double>(1);

	m_minibatchStateVector = std::vector<double>(m_numberOfStateVars * m_experienceReplay->getUpdateBatchSize(), 0.0);
	m_minibatchActionVector = std::vector<double>(1 * m_experienceReplay->getUpdateBatchSize(), 0.0);
	m_minibatchQVector = std::vector<double>(1 * m_experienceReplay->getUpdateBatchSize(), 0.0);

	m_pTargetQNetwork = m_predictionQNetwork.getNetwork()->cloneNonTrainable();
	m_pTargetPolicyNetwork = m_predictionPolicyNetwork.getNetwork()->cloneNonTrainable();

	buildModelsParametersTransitionGraph();
}

double DDPG::selectAction(const State * s, Action * a)
{
	//get the state vector
	SimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(s, m_pStateFeatureList);
	for (int i = 0; i < m_pStateFeatureList->m_numFeatures; i++)
		m_stateVector[m_pStateFeatureList->m_pFeatures[i].m_index] = m_pStateFeatureList->m_pFeatures[i].m_factor;

	std::unordered_map<std::string, std::vector<double>&> inputMap = 
		{ { "state-input", m_stateVector } };
	m_predictionPolicyNetwork.getNetwork()->predict(inputMap, m_actionPredictionVector);

	double actionValue = m_actionPredictionVector[0];
	actionValue += m_policyNoise->getSample();
	a->set(m_outputActionIndex.get(), actionValue);

	return 1.0;
}

double DDPG::update(const State * s, const Action * a, const State * s_p, double r, double behaviorProb)
{
	double gamma = SimionApp::get()->pSimGod->getGamma();

	m_experienceReplay->addTuple(s, a, s_p, r, 1.0);

	//don't update anything if the experience replay buffer does not contain enough elements for at least one minibatch
	if (m_experienceReplay->getUpdateBatchSize() != m_experienceReplay->getUpdateBatchSize())
		return 0.0;

	for (int i = 0; i < m_experienceReplay->getUpdateBatchSize(); i++)
	{
		m_pMinibatchExperienceTuples[i] = m_experienceReplay->getRandomTupleFromBuffer();
	}

	updateCritic();
	updateActor();

	performModelsParametersTransition();

	return 0.0;
}

void DDPG::updateActor()
{
	CNTK::FunctionPtr modelPolicyOutputPtr = m_predictionPolicyNetwork.getNetwork()->getOutputsFunctionPtr()[0];
	CNTK::FunctionPtr modelQOutputPtr = m_predictionQNetwork.getNetwork()->getOutputsFunctionPtr()[0];

	for (int i = 0; i < m_experienceReplay->getUpdateBatchSize(); i++)
	{
		//get Q(s_p) for entire minibatch
		SimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(m_pMinibatchExperienceTuples[i]->s, m_pStateFeatureList);
		for (int n = 0; n < m_pStateFeatureList->m_numFeatures; n++)
			m_minibatchStateVector[m_pStateFeatureList->m_pFeatures[n].m_index + i * m_numberOfStateVars] = m_pStateFeatureList->m_pFeatures[n].m_factor;

		m_minibatchActionVector[i] = m_pMinibatchExperienceTuples[i]->a->get(m_outputActionIndex.get());
	}

	CNTK::ValuePtr stateInputValue = CNTK::Value::CreateBatch(m_predictionQNetwork.getNetwork()->getInputs()[0]->getInputVariable().Shape(), m_minibatchStateVector, CNTK::DeviceDescriptor::CPUDevice());

	//get gradient of Q
	CNTK::ValuePtr chosenActionValuePtr;
	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> getChoosenActionInputMap =
	{
		{ m_predictionPolicyNetwork.getNetwork()->getInputs()[0]->getInputVariable()
		, stateInputValue }
	};

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> chosenActionOutputMap = { { modelPolicyOutputPtr, chosenActionValuePtr } };
	m_predictionPolicyNetwork.getNetwork()->getNetworkFunctionPtr()->Evaluate(getChoosenActionInputMap, chosenActionOutputMap, CNTK::DeviceDescriptor::CPUDevice());
	chosenActionValuePtr = chosenActionOutputMap[modelPolicyOutputPtr];

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> qGradientActionArgument = { { m_predictionQNetwork.getNetwork()->getInputs()[0]->getInputVariable(), stateInputValue } ,
		{ m_predictionQNetwork.getNetwork()->getInputs()[1]->getInputVariable(), chosenActionValuePtr } };
	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> qGradientOutputMap =
		{ { m_predictionQNetwork.getNetwork()->getInputs()[1]->getInputVariable(), nullptr } };
	modelQOutputPtr->Gradients(qGradientActionArgument, qGradientOutputMap, CNTK::DeviceDescriptor::CPUDevice());

	//get the gradient of mu/the policy
	//forward propagation of values
	//therefore, fill up the input variables
	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> policyInputMap;
	policyInputMap[m_predictionPolicyNetwork.getNetwork()->getInputs()[0]->getInputVariable()] = stateInputValue;

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> policyOutputMap;
	policyOutputMap[m_predictionPolicyNetwork.getNetwork()->getOutputsFunctionPtr()[0]->Output()] = nullptr;

	//perform now the forward propagation
	auto backPropState = modelPolicyOutputPtr->Forward(policyInputMap, policyOutputMap, CNTK::DeviceDescriptor::CPUDevice(), { modelPolicyOutputPtr });
	CNTK::ValuePtr policyOutputValue = policyOutputMap[m_predictionPolicyNetwork.getNetwork()->getOutputsFunctionPtr()[0]->Output()];

	CNTK::ValuePtr rootGradientValue = qGradientOutputMap[m_predictionQNetwork.getNetwork()->getInputs()[1]->getInputVariable()];

	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> parameterGradients;
	for (const auto& parameter : modelPolicyOutputPtr->Parameters())
		//This is not working: set the gradients to the Q gradient to multipy them with the current value
		//therefore, use nullptr
		parameterGradients[parameter] = CNTK::ValuePtr();
	//calculate grad_q * grad_policy for each parameter's weight
	modelPolicyOutputPtr->Backward(backPropState, { { modelPolicyOutputPtr, rootGradientValue } }, parameterGradients);

	//update the parameters of the policy now
	std::unordered_map<CNTK::Parameter, CNTK::NDArrayViewPtr> gradients;
	for (const auto& parameter : modelPolicyOutputPtr->Parameters())
		gradients[parameter] = parameterGradients[parameter]->Data();

	m_predictionPolicyNetwork.getNetwork()->getTrainer()->ParameterLearners()[0]->Update(gradients, m_experienceReplay->getUpdateBatchSize());

	
	//we have to perform something like this

	//self.q_gradient_input = tf.placeholder("float",[None,self.action_dim])
	//self.parameters_gradients = tf.gradients(self.action_output,self.net,-self.q_gradient_input)
	//self.optimizer = tf.train.AdamOptimizer(LEARNING_RATE).apply_gradients(zip(self.parameters_gradients,self.net))

	//maybe this helps
	//https://stackoverflow.com/questions/41814858/how-to-access-gradients-and-modify-weights-parameters-directly-during-training

	//python: Learner.update(gradient_values, training_sample_count)
	//C++:    model->getTrainer()->ParameterLearners()[0]->Update();
	
}

void DDPG::updateCritic()
{
	double gamma = SimionApp::get()->pSimGod->getGamma();

	for (int i = 0; i < m_experienceReplay->getUpdateBatchSize(); i++)
	{
		SimionApp::get()->pSimGod->getGlobalStateFeatureMap()->getFeatures(m_pMinibatchExperienceTuples[i]->s_p, m_pStateFeatureList);
		for (int n = 0; n < m_pStateFeatureList->m_numFeatures; n++)
			m_minibatchStateVector[m_pStateFeatureList->m_pFeatures[n].m_index + i * m_numberOfStateVars] = m_pStateFeatureList->m_pFeatures[n].m_factor;
	}

	//calculate mu'(s_p)
	std::unordered_map<std::string, vector<double>&> inputMap =
		{ { "state-input", m_minibatchStateVector } };
	m_pTargetPolicyNetwork->predict(inputMap, m_minibatchActionVector);

	//calculate Q'(mu'(s_p)) now
	inputMap = 
		{ 
			{ "state-input", m_minibatchStateVector }
			,{ "action-input", m_minibatchActionVector } 
		};
	m_pTargetQNetwork->predict(inputMap, m_minibatchQVector);

	//calculate y_i
	for (int i = 0; i < m_experienceReplay->getUpdateBatchSize(); i++)
	{
		m_minibatchQVector[i] = m_pMinibatchExperienceTuples[i]->r + gamma * m_minibatchQVector[i];
	}

	//update the network finally
	for (int i = 0; i < m_experienceReplay->getUpdateBatchSize(); i++)
	{
		m_minibatchActionVector[i] = m_pMinibatchExperienceTuples[i]->a->get(m_outputActionIndex.get());
	}

	inputMap = 
	{ 
		{ "state-input", m_minibatchStateVector }
		,{ "action-input", m_minibatchActionVector }
	};

	m_predictionQNetwork.getNetwork()->train(inputMap, m_minibatchQVector);
}
*/
#endif