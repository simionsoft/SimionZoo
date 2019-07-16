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
#include "deep-minibatch.h"
#include "parameters.h"
#include <algorithm>
#include "deep-vfa-policy.h"
#include "config.h"
#include "deep-network.h"

DQN::~DQN()
{
	if (m_pTargetQNetwork) m_pTargetQNetwork->destroy();
	if (m_pOnlineQNetwork) m_pOnlineQNetwork->destroy();
	if (m_pMinibatch) delete m_pMinibatch;
	CNTK::WrapperClient::UnLoad();
}

DQN::DQN(ConfigNode* pConfigNode)
{
	m_pQFunction = CHILD_OBJECT<DeepDiscreteQFunction>(pConfigNode, "Q-Network", "The definition of the Q-function learned by the agent");

	//Only register dependencies. The wrapper is loaded on the deferred load step
	CNTK::WrapperClient::RegisterDependencies();

	m_policy = CHILD_OBJECT_FACTORY<DiscreteDeepPolicy>(pConfigNode, "Policy", "The policy");
}

void DQN::deferredLoadStep()
{
	//we defer all the heavy-weight initializing stuff and anything that depends on the SimGod
	
	//Load Cntk if we must
	CNTK::WrapperClient::Load();

	//create the networks
	m_pOnlineQNetwork = m_pQFunction->getNetworkInstance();
	SimionApp::get()->registerStateActionFunction("Q", m_pOnlineQNetwork);
	m_pTargetQNetwork = (IDiscreteQFunctionNetwork*) m_pOnlineQNetwork->clone();

	m_pMinibatch = m_pQFunction->getMinibatch();

	//initialize the policy
	m_policy->initialize(m_pQFunction.ptr());

	//auxiliary vectors
	m_Q_s_p = vector<double>(m_pQFunction->getNumOutputs() * m_pMinibatch->size());
	m_argMaxIndex = vector<int>(m_pMinibatch->size());
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

IDiscreteQFunctionNetwork* DQN::getTargetNetwork()
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
		getTargetNetwork()->evaluate(m_pMinibatch->s_p(), m_Q_s_p);

		//Find the index of the action with max Q-value for each tuple in the minibatch
		for (int i = 0; i < m_pMinibatch->size(); i++)
		{
			//calculate arg max Q(s_p, a)
			vector<double>::iterator itBegin = m_Q_s_p.begin() + i * m_pQFunction->getNumOutputs();
			vector<double>::iterator itEnd = m_Q_s_p.begin() + (i + 1) * m_pQFunction->getNumOutputs();
			m_argMaxIndex[i] = (int)distance(itBegin, max_element(itBegin, itEnd));
		}

		//estimate Q(s_p, argMaxQ; target-weights or online-weights)
		//THIS is the only real difference between DQN and Double-DQN
		//We do the prediction step again only if using Double-DQN (the prediction network
		//will be different to the online network)
		if (getTargetNetwork() != m_pTargetQNetwork)
			getTargetNetwork()->evaluate(m_pMinibatch->s(), m_Q_s_p);

		//get the value of Q(s) for each tuple
		m_pOnlineQNetwork->evaluate(m_pMinibatch->s(), m_pMinibatch->target());

		//for each tuple in the minibatch
		for (int i = 0; i < m_pMinibatch->size(); i++)
		{
			//calculate targetvalue= r + gamma*Q(s_p,a)
			double targetValue = m_pMinibatch->r()[i] + gamma * m_Q_s_p[m_argMaxIndex[i]];

			//change the target value only for the selected action, the rest remain the same
			//store the index of the action taken
			size_t selectedActionId = m_policy->getActionIndex( m_pMinibatch->a() , i );
			m_pMinibatch->target() [i * m_pQFunction->getNumOutputs() + selectedActionId ] = targetValue;
		}

		//train the network
		m_pOnlineQNetwork->train(m_pMinibatch, m_pMinibatch->target(), m_pQFunction->getLearningRate());

		m_pMinibatch->clear();
	}

	SimGod* pSimGod = SimionApp::get()->pSimGod.ptr();

	if (pSimGod->bUpdateFrozenWeightsNow())
	{
		if (m_pTargetQNetwork)
			m_pTargetQNetwork->destroy();
			m_pTargetQNetwork = (IDiscreteQFunctionNetwork*) m_pOnlineQNetwork->clone();
	}

	return 1.0; //TODO: Estimate the TD-error??
}


DoubleDQN::DoubleDQN(ConfigNode* pParameters): DQN (pParameters)
{}

IDiscreteQFunctionNetwork* DoubleDQN::getTargetNetwork()
{
	return m_pOnlineQNetwork;
}

#endif