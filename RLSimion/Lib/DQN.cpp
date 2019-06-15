/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "DQN.h"

#if defined(__linux__) || defined(_WIN64)

#include "simgod.h"
#include "logger.h"
#include "worlds/world.h"
#include "app.h"
#include "noise.h"
#include "deep-vfa-policy.h"
#include "parameters-numeric.h"
#include "parameters.h"
#include <algorithm>
#include "deep-vfa-policy.h"
#include "config.h"

#include "../CNTKWrapper/CNTKWrapper.h"


DQN::~DQN()
{
	//We need to manually call the NN_DEFINITION destructor
	m_pNNDefinition->destroy();
	if (m_pTargetQNetwork) m_pTargetQNetwork->destroy();
	if (m_pOnlineQNetwork) m_pOnlineQNetwork->destroy();
	if (m_pMinibatch) m_pMinibatch->destroy();
	CNTK::WrapperClient::UnLoad();
}

DQN::DQN(ConfigNode* pConfigNode)
{
	m_minibatchSize = INT_PARAM(pConfigNode, "Minibatch-Size", "The size in tuples of the minibatch", 100);
	m_inputState = MULTI_VALUE_VARIABLE<STATE_VARIABLE>(pConfigNode, "Input-State", "Set of variables used as input of the QNetwork");
	m_numActionSteps = INT_PARAM(pConfigNode, "Num-Action-Steps", "Number of discrete step used for each output action", 10);
	m_outputAction = MULTI_VALUE_VARIABLE<ACTION_VARIABLE>(pConfigNode, "Output-Action", "The output action variables");
	m_learningRate = DOUBLE_PARAM(pConfigNode, "Learning-Rate", "The learning rate at which the agent learns", 0.000001);

	//The wrapper must be initialized before loading the NN definition
	CNTK::WrapperClient::Load();
	m_policy = CHILD_OBJECT_FACTORY<DiscreteDeepPolicy>(pConfigNode, "Policy", "The policy");
	m_pNNDefinition = NN_DEFINITION(pConfigNode, "neural-network", "Neural Network Architecture");
}

void DQN::deferredLoadStep()
{
	//we defer all the heavy-weight initializing stuff and anything that depends on the SimGod
	
	//set the input-outputs
	for (size_t i = 0; i < m_inputState.size(); ++i)
		m_pNNDefinition->addInputStateVar(m_inputState[i]->get());
	//Set the output actions as input to the network
	for (size_t i = 0; i < m_outputAction.size(); i++)
		m_pNNDefinition->addInputActionVar(m_outputAction[i]->get());

	m_pNNDefinition->setScalarOutput();

	//create the networks
	m_pOnlineQNetwork = m_pNNDefinition->createNetwork(m_learningRate.get());
	SimionApp::get()->registerStateActionFunction("Q", m_pOnlineQNetwork);
	m_pTargetQNetwork = m_pOnlineQNetwork->clone();

	//create the minibatch
	//The size of the minibatch is the experience replay update size
	//This is because we only perform updates in replaying-experience mode
	if ((int)SimionApp::get()->pSimGod->getExperienceReplayUpdateSize() == 0)
		Logger::logMessage(MessageType::Error, "Both DQN and Double-DQN require the use of Experience Replay");
	if (m_minibatchSize.get() <= 1)
		Logger::logMessage(MessageType::Warning, "It is recommended to use a minimum minibatch size of 100 for better performance");

	m_pMinibatch = m_pNNDefinition->createMinibatch(m_minibatchSize.get());

	//initialize the policy
	m_policy->initialize( m_pOnlineQNetwork->getDefinition(), SimionApp::get()->pWorld->getDynamicModel()->getActionDescriptor(), m_numActionSteps.get());

	m_Q_s_p = vector<double>(m_minibatchSize.get());
	m_target = vector<double>(m_minibatchSize.get());
}

/// <summary>
/// Implements the action selection algorithm for a Q-based Deep RL algorithm
/// </summary>
/// <param name="s">State</param>
/// <param name="a">Output action</param>
double DQN::selectAction(const State * s, Action * a)
{
	m_policy->selectAction(m_pOnlineQNetwork, s, a);

	return 1.0;
}

INetwork* DQN::getTargetNetwork()
{
	return m_pTargetQNetwork;
}

/// <summary>
/// Implements DQL algorithm update using only one Neural Network for both evaluation and update
/// </summary>
/// <param name="s">Initial state</param>
/// <param name="a">Action</param>
/// <param name="s_p">Resultant state</param>
/// <param name="r">Reward</param>
double DQN::update(const State * s, const Action * a, const State * s_p, double r, double behaviorProb)
{
	if (!m_pMinibatch->isFull())
	{
		m_pMinibatch->addTuple(s, a, s_p, r);
	}

	if (m_pMinibatch->isFull())
	{
		//Minibatch is full: ready for training
		double gamma = SimionApp::get()->pSimGod->getGamma();

		//Calculate arg max a' Q(s_p,a') for all the tuples in the minibatch (target/online-weights)

		for (int tuple= 0; tuple<m_pMinibatch->size(); tuple++)
			m_policy->maxValue(m_pTargetQNetwork, m_pMinibatch->s_p(), &m_Q_s_p[tuple], tuple);

		//estimate Q(s_p, argMaxQ; target-weights or online-weights)
		//THIS is the only real difference between DQN and Double-DQN
		//We do the prediction step again only if using Double-DQN (the prediction network
		//will be different to the online network)
		if (getTargetNetwork() != m_pTargetQNetwork)
			m_pTargetQNetwork->evaluate(m_pMinibatch->interleavedStateAction(), m_pMinibatch->a(), m_Q_s_p);

		//get the value of Q(s) for each tuple
		m_pOnlineQNetwork->evaluate(m_pMinibatch->interleavedStateAction(), m_pMinibatch->a(), m_target);

		//for each tuple in the minibatch
		for (int i = 0; i < m_minibatchSize.get(); i++)
		{
			//calculate targetvalue= r + gamma*Q(s_p,a)
			m_target[i] = m_pMinibatch->r()[i] + gamma * m_Q_s_p[i];
		}

		//train the network
		m_pOnlineQNetwork->train(m_pMinibatch, m_target);

		m_pMinibatch->clear();
	}

	SimGod* pSimGod = SimionApp::get()->pSimGod.ptr();

	if (!pSimGod->bReplayingExperience() && pSimGod->bUpdateFrozenWeightsNow())
	{
		if (m_pTargetQNetwork)
			m_pTargetQNetwork->destroy();
			m_pTargetQNetwork = m_pOnlineQNetwork->clone();
	}

	return 1.0; //TODO: Estimate the TD-error??
}


DoubleDQN::DoubleDQN(ConfigNode* pParameters): DQN (pParameters)
{}

INetwork* DoubleDQN::getTargetNetwork()
{
	return m_pOnlineQNetwork;
}

#endif