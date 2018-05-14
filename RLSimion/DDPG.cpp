#include "DDPG.h"

#ifdef _WIN64

#include "../tools/CNTKWrapper/CNTKWrapper.h"
#include "app.h"
#include "noise.h"
#include "simgod.h"
#include "logger.h"
#include "experiment.h"
#include "worlds\world.h"

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

	delete m_pActorOutput;
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

	m_pActorOutput = SimionApp::get()->pWorld->getDynamicModel()->getActionInstance();

	//Set the state-input
	for (size_t stateVarIndex = 0; stateVarIndex < m_inputState.size(); stateVarIndex++)
	{
		m_CriticNetworkDefinition->addInputStateVar(m_inputState[stateVarIndex]->getName(), m_inputState[stateVarIndex]->get());
		m_ActorNetworkDefinition->addInputStateVar(m_inputState[stateVarIndex]->getName(), m_inputState[stateVarIndex]->get());
	}

	//Set the action-input: for now, only the one used as output of the policy
	for (size_t actionVarIndex = 0; actionVarIndex < m_outputAction.size(); actionVarIndex++)
	{
		m_CriticNetworkDefinition->addInputActionVar(m_outputAction[actionVarIndex]->getName(), m_outputAction[actionVarIndex]->get());
	}
	m_gradientWrtAction = vector<double>(m_outputAction.size());

	//Set critic networks as single-output
	m_CriticNetworkDefinition->setScalarOutput();
	//Set actor network as vectorial output with the dimension=numActions
	m_ActorNetworkDefinition->setVectorOutput(m_outputAction.size());

	//Critic initialization
	m_pCriticOnlineNetwork = m_CriticNetworkDefinition->createNetwork(m_learningRate.get(), true); //true because we are going to need gradient calculations for this network
	SimionApp::get()->registerStateActionFunction("Q", m_pCriticOnlineNetwork);
	m_pCriticTargetNetwork = m_pCriticOnlineNetwork->clone(false);
	m_pCriticTargetNetwork->initSoftUpdate(m_tau.get(), m_pCriticOnlineNetwork);
	m_pCriticMinibatch = m_CriticNetworkDefinition->createMinibatch(minibatchSize);

	//Actor initialization
	m_pActorOnlineNetwork = m_ActorNetworkDefinition->createNetwork(m_learningRate.get());
	SimionApp::get()->registerStateActionFunction("Policy", m_pActorOnlineNetwork);
	m_pActorTargetNetwork = m_pActorOnlineNetwork->clone(false);
	m_pActorTargetNetwork->initSoftUpdate(m_tau.get(), m_pActorOnlineNetwork);
	
	//The size of the target in the minibatch has to match the number of actions to save the gradient wrt an action
	m_pActorMinibatch = m_ActorNetworkDefinition->createMinibatch(minibatchSize, m_outputAction.size());
}

double DDPG::selectAction(const State * s, Action * a)
{
	double policyOutput;
	vector<double>& actionValues = m_pActorOnlineNetwork->evaluate(s, a);
	for (size_t i = 0; i < m_outputAction.size(); i++)
	{
		policyOutput = actionValues[i];
		if (!SimionApp::get()->pExperiment->isEvaluationEpisode())
			policyOutput+= m_policyNoise->getSample();
		a->set(m_outputAction[i]->get(), policyOutput);
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

		//get pi(s)
		vector<double>& actionValues = m_pActorTargetNetwork->evaluate(s, a);

		//a' = pi(s)
		for (size_t i = 0; i < m_outputAction.size(); i++)
			m_pActorOutput->set(m_outputAction[i]->get(), actionValues[i]);

		//gradient = critic->gradient(s, pi(s))
		m_pCriticTargetNetwork->gradientWrtAction(s, m_pActorOutput, m_gradientWrtAction);

		//gradient = -gradient
		for (size_t i = 0; i < m_outputAction.size(); i++)
			m_gradientWrtAction[i] *= -1.0;

		m_pActorMinibatch->addTuple(s, m_pActorOutput, m_gradientWrtAction);
	}
	else if (m_pActorMinibatch->isFull())
	{
		m_pActorOnlineNetwork->applyGradient(m_pActorMinibatch);

		//if (SimionApp::get()->pExperiment->getExperimentStep() % 10)
		//{
		//	m_pActorTargetNetwork->destroy();
		//	m_pActorTargetNetwork = m_pActorOnlineNetwork->clone();
		//}

		m_pActorTargetNetwork->softUpdate(m_pActorOnlineNetwork);
	}
}

void DDPG::updateCritic(const State* s, const Action* a, const State* s_p, double r)
{
	if (SimionApp::get()->pSimGod->bReplayingExperience())
	{
		double gamma = SimionApp::get()->pSimGod->getGamma();

		//calculate pi(s_p)
		vector<double>& actionValues = m_pActorTargetNetwork->evaluate(s_p, a);

		//a' = pi(s_p)
		for (size_t i = 0; i < m_outputAction.size(); i++)
			m_pActorOutput->set(m_outputAction[i]->get(), actionValues[i]);
		
		//calculate Q'(mu'(s_p))
		double s_p_value = m_pCriticTargetNetwork->evaluate(s_p, m_pActorOutput)[0]; //we assume only the critic will have only one output

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
		//m_pCriticTargetNetwork->softUpdate(m_pCriticOnlineNetwork);

		if (SimionApp::get()->pExperiment->getExperimentStep() % 10)
		{
			m_pCriticTargetNetwork->destroy();
			m_pCriticTargetNetwork = m_pCriticOnlineNetwork->clone();
		}
	}
}


#endif