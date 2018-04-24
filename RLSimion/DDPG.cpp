#include "DDPG.h"
#include "../tools/CNTKWrapper/CNTKWrapper.h"
#include "app-rlsimion.h"
#include "noise.h"

DDPG::~DDPG()
{
	m_CriticNetworkDefinition->destroy();
	m_pCriticOnlineNetwork->destroy();
	m_pCriticTargetNetwork->destroy();
	m_pCriticMinibatch->destroy();

	m_ActorNetworkDefinition->destroy();
	m_pActorOnlineNetwork->destroy();
	m_pActorTargetNetwork->destroy();
	m_pActorMinibatch->destroy();

	CNTKWrapperLoader::UnLoad();
}


DDPG::DDPG(ConfigNode * pConfigNode)
{
	//The wrapper must be initialized before loading the NN definition
	CNTKWrapperLoader::Load();

	m_CriticNetworkDefinition = NN_DEFINITION(pConfigNode, "Critic-Network", "Neural Network for the Critic -a Q function-");
	m_ActorNetworkDefinition = NN_DEFINITION(pConfigNode, "Actor-Network", "Neural Network for the Actor -deterministic policy-");
	m_policyNoise = CHILD_OBJECT_FACTORY<Noise>(pConfigNode, "Exploration-Noise", "Noise added to the output of the policy", false);
	m_tau = DOUBLE_PARAM(pConfigNode, "Tau", "The rate by which the target weights approach the online weights", 0.001);

	m_inputState = MULTI_VALUE_VARIABLE<STATE_VARIABLE>(pConfigNode, "Input-State", "Set of state variables used as input");
	m_outputAction = MULTI_VALUE_VARIABLE<ACTION_VARIABLE>(pConfigNode, "Output-Action", "The output action variable");
	m_learningRate = DOUBLE_PARAM(pConfigNode, "Learning-Rate", "The learning rate at which the agent learns", 0.000001);
}

void DDPG::deferredLoadStep()
{
	size_t minibatchSize = SimionApp::get()->pSimGod->getExperienceReplayUpdateSize();
	if (minibatchSize == 0)
		Logger::logMessage(MessageType::Error, "DDPG requires the use of the Experience Replay Buffer technique");

	//Set the state-input
	for (size_t stateVarIndex = 0; stateVarIndex < m_inputState.size(); stateVarIndex++)
	{
		m_CriticNetworkDefinition->addInputStateVar(m_inputState[stateVarIndex]->get());
		m_ActorNetworkDefinition->addInputStateVar(m_inputState[stateVarIndex]->get());
	}
	m_stateValues = vector<double>(m_inputState.size());

	//Set the action-input: for now, only the one used as output of the policy
	for (size_t actionVarIndex = 0; actionVarIndex < m_outputAction.size(); actionVarIndex++)
	{
		m_CriticNetworkDefinition->addInputActionVar(m_outputAction[actionVarIndex]->get());
	}
	m_actionValues = vector<double>(m_outputAction.size());
	m_gradientWrtAction = vector<double>(m_outputAction.size());

	//Set both networks as single-output
	m_CriticNetworkDefinition->setScalarOutput();
	m_ActorNetworkDefinition->setVectorOutput(m_outputAction.size());

	//Critic initialization
	m_pCriticOnlineNetwork = m_CriticNetworkDefinition->createNetwork(m_learningRate.get());
	m_pCriticTargetNetwork = m_pCriticOnlineNetwork->clone(false);
	m_pCriticTargetNetwork->initSoftUpdate(m_tau.get(), m_pCriticOnlineNetwork);
	m_pCriticMinibatch = m_CriticNetworkDefinition->createMinibatch(minibatchSize);

	//Actor initialization
	m_pActorOnlineNetwork = m_ActorNetworkDefinition->createNetwork(m_learningRate.get());
	m_pActorTargetNetwork = m_pActorOnlineNetwork->clone(false);
	m_pActorTargetNetwork->initSoftUpdate(m_tau.get(), m_pActorOnlineNetwork);
	
	//1 entry for each action in the minibatch to save the gradient wrt a
	m_pActorMinibatch = m_ActorNetworkDefinition->createMinibatch(minibatchSize, m_outputAction.size());
}

double DDPG::selectAction(const State * s, Action * a)
{
	m_pActorOnlineNetwork->get(s, a);
	for (size_t i = 0; i < m_outputAction.size(); i++)
	{
		double policyOutput = a->get(m_outputAction[i]->get());
		double noise = m_policyNoise->getSample();
		a->set(m_outputAction[i]->get(), policyOutput + noise);
	}
	return 1.0;
}

double DDPG::update(const State * s, const Action * a, const State * s_p, double r, double behaviorProb)
{
	updateCritic(s, a, s_p, r);
	updateActor(s, a, s_p, r);
	
	return 0.0;
}

void DDPG::updateActor(const State* s, const Action* a, const State* s_p, double r)
{
	if (SimionApp::get()->pSimGod->bReplayingExperience())
	{
		double gamma = SimionApp::get()->pSimGod->getGamma();

		m_pActorTargetNetwork->get(s, a, m_actionValues);
		//m_pCriticTargetNetwork->gradient(s, a, m_gradientWrtAction);
	}
	else if (m_pActorMinibatch->isFull())
	{

	}

	//CNTK::ValuePtr stateInputValue = CNTK::Value::CreateBatch(m_predictionQNetwork.getNetwork()->getInputs()[0]->getInputVariable().Shape(), m_minibatch_s, CNTK::DeviceDescriptor::CPUDevice());

	////get gradient of Q
	//CNTK::ValuePtr chosenActionValuePtr;
	//unordered_map<CNTK::Variable, CNTK::ValuePtr> getChoosenActionInputMap =
	//{
	//	{ m_predictionPolicyNetwork.getNetwork()->getInputs()[0]->getInputVariable()
	//	, stateInputValue }
	//};

	//unordered_map<CNTK::Variable, CNTK::ValuePtr> chosenActionOutputMap = { { modelPolicyOutputPtr, chosenActionValuePtr } };
	//m_predictionPolicyNetwork.getNetwork()->getNetworkFunctionPtr()->Evaluate(getChoosenActionInputMap, chosenActionOutputMap, CNTK::DeviceDescriptor::CPUDevice());
	//chosenActionValuePtr = chosenActionOutputMap[modelPolicyOutputPtr];

	//unordered_map<CNTK::Variable, CNTK::ValuePtr> qGradientActionArgument = { { m_predictionQNetwork.getNetwork()->getInputs()[0]->getInputVariable(), stateInputValue } ,
	//{ m_predictionQNetwork.getNetwork()->getInputs()[1]->getInputVariable(), chosenActionValuePtr } };
	//unordered_map<CNTK::Variable, CNTK::ValuePtr> qGradientOutputMap =
	//{ { m_predictionQNetwork.getNetwork()->getInputs()[1]->getInputVariable(), nullptr } };
	//modelQOutputPtr->Gradients(qGradientActionArgument, qGradientOutputMap, CNTK::DeviceDescriptor::CPUDevice());

	////get the gradient of mu/the policy
	////forward propagation of values
	////therefore, fill up the input variables
	//unordered_map<CNTK::Variable, CNTK::ValuePtr> policyInputMap;
	//policyInputMap[m_predictionPolicyNetwork.getNetwork()->getInputs()[0]->getInputVariable()] = stateInputValue;

	//unordered_map<CNTK::Variable, CNTK::ValuePtr> policyOutputMap;
	//policyOutputMap[m_predictionPolicyNetwork.getNetwork()->getOutputsFunctionPtr()[0]->Output()] = nullptr;

	////perform now the forward propagation
	//auto backPropState = modelPolicyOutputPtr->Forward(policyInputMap, policyOutputMap, CNTK::DeviceDescriptor::CPUDevice(), { modelPolicyOutputPtr });
	//CNTK::ValuePtr policyOutputValue = policyOutputMap[m_predictionPolicyNetwork.getNetwork()->getOutputsFunctionPtr()[0]->Output()];

	//CNTK::ValuePtr rootGradientValue = qGradientOutputMap[m_predictionQNetwork.getNetwork()->getInputs()[1]->getInputVariable()];

	//unordered_map<CNTK::Variable, CNTK::ValuePtr> parameterGradients;
	//for (const auto& parameter : modelPolicyOutputPtr->Parameters())
	////This is not working: set the gradients to the Q gradient to multipy them with the current value
	////therefore, use nullptr
	//parameterGradients[parameter] = CNTK::ValuePtr();
	////calculate grad_q * grad_policy for each parameter's weight
	//modelPolicyOutputPtr->Backward(backPropState, { { modelPolicyOutputPtr, rootGradientValue } }, parameterGradients);

	////update the parameters of the policy now
	//unordered_map<CNTK::Parameter, CNTK::NDArrayViewPtr> gradients;
	//for (const auto& parameter : modelPolicyOutputPtr->Parameters())
	//gradients[parameter] = parameterGradients[parameter]->Data();

	//m_predictionPolicyNetwork.getNetwork()->getTrainer()->ParameterLearners()[0]->Update(gradients, m_experienceReplay->getUpdateBatchSize());
	m_pActorTargetNetwork->softUpdate(m_pActorOnlineNetwork);


	//we have to perform something like this

	//self.q_gradient_input = tf.placeholder("float",[None,self.action_dim])
	//self.parameters_gradients = tf.gradients(self.action_output,self.net,-self.q_gradient_input)
	//self.optimizer = tf.train.AdamOptimizer(LEARNING_RATE).apply_gradients(zip(self.parameters_gradients,self.net))

	//maybe this helps
	//https://stackoverflow.com/questions/41814858/how-to-access-gradients-and-modify-weights-parameters-directly-during-training

	//python: Learner.update(gradient_values, training_sample_count)
	//C++:    model->getTrainer()->ParameterLearners()[0]->Update();

}

void DDPG::updateCritic(const State* s, const Action* a, const State* s_p, double r)
{
	if (SimionApp::get()->pSimGod->bReplayingExperience())
	{
		double gamma = SimionApp::get()->pSimGod->getGamma();

		//calculate mu'(s_p)
		double policyOutput= m_pActorTargetNetwork->get(s_p, a);
		
		//calculate Q'(mu'(s_p))
		double s_p_value = m_pCriticTargetNetwork->get(s_p, a);

		//calculate targetvalue= r + gamma*Q(s_p,a)
		double targetValue = r + gamma * s_p_value;

		//add a new tuple to the minibatch
		m_pCriticMinibatch->addTuple(s, a, targetValue);
	}
	//We only train the network in direct-experience updates to simplify mini-batching
	else if (m_pCriticMinibatch->isFull())
	{
		SimGod* pSimGod = SimionApp::get()->pSimGod.ptr();

		//update the network finally
		m_pCriticOnlineNetwork->train(m_pCriticMinibatch);

		//move the target weights toward the online weights
		m_pCriticTargetNetwork->softUpdate(m_pCriticOnlineNetwork);
	}
}
