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

#include "DDPG.h"
#if defined(__linux__) || defined(_WIN64)
#include "deep-network.h"
#include "deep-minibatch.h"
#include "deep-functions.h"
#include "app.h"
#include "noise.h"
#include "simgod.h"
#include "logger.h"
#include "experiment.h"
#include "worlds/world.h"
#include "cntk-wrapper-loader.h"
#include <algorithm>

DDPG::DDPG(ConfigNode * pConfigNode) : Simion(pConfigNode)
{
	m_actorPolicy= CHILD_OBJECT<DeepDeterministicPolicy>(pConfigNode, "Policy", "Policy represented as a Neural Network learned by the actor");
	m_noiseSignals = MULTI_VALUE_FACTORY<Noise>(pConfigNode, "Exploration-Noise", "Noise signals added to each of the outputs of the deterministic policy");
	m_criticQFunction= CHILD_OBJECT<DeepContinuousQFunction>(pConfigNode, "Q-Value-Function", "Value function learned by the critic");
	m_tau = DOUBLE_PARAM(pConfigNode, "Tau", "Parameter controlling the soft-updates of the online network", 0.001);

	CNTK::WrapperLoader::SetRequirements();
}

DDPG::~DDPG()
{
	if (m_pCriticMinibatch) delete m_pCriticMinibatch;
	if (m_pCriticOnlineNetwork != nullptr)
		m_pCriticOnlineNetwork->destroy();
	if (m_pCriticTargetNetwork != nullptr)
		m_pCriticTargetNetwork->destroy();

	if (m_pActorMinibatch) delete m_pActorMinibatch;
	if (m_pActorOnlineNetwork != nullptr)
		m_pActorOnlineNetwork->destroy();
	if (m_pActorTargetNetwork != nullptr)
		m_pActorTargetNetwork->destroy();

	CNTK::WrapperLoader::UnLoad();
}

void DDPG::deferredLoadStep()
{
	CNTK::WrapperLoader::Load();

	//Initialize the actor
	m_pActorMinibatch = m_actorPolicy->getMinibatch();
	m_pActorOnlineNetwork = m_actorPolicy->getNetworkInstance();
	m_pActorTargetNetwork = (IDeterministicPolicyNetwork*) m_pActorOnlineNetwork->clone(false);
	SimionApp::get()->registerStateActionFunction("Online Policy", m_pActorOnlineNetwork);
	SimionApp::get()->registerStateActionFunction("Target Policy", m_pActorTargetNetwork);

	m_pi_s = vector<double>(m_actorPolicy->getNumOutputs() * m_pActorMinibatch->size());
	m_pi_s_p = vector<double>(m_actorPolicy->getNumOutputs() * m_pActorMinibatch->size());
	m_gradient = vector<double>(m_actorPolicy->getNumOutputs() * m_pActorMinibatch->size());

	//Initialise the critic
	m_pCriticMinibatch = m_criticQFunction->getMinibatch();
	m_pCriticOnlineNetwork = m_criticQFunction->getNetworkInstance();
	m_pCriticTargetNetwork = (IContinuousQFunctionNetwork*) m_pCriticOnlineNetwork->clone(false);
	SimionApp::get()->registerStateActionFunction("Online Q", m_pCriticOnlineNetwork);
	SimionApp::get()->registerStateActionFunction("Target Q", m_pCriticTargetNetwork);

	m_Q_pi_s_p = vector<double>(m_pCriticMinibatch->size());
}

/// <summary>
/// Implements action selection for the DDPG algorithm adding the output of the actor and exploration noise signal
/// </summary>
/// <param name="s">State</param>
/// <param name="a">Output action</param>
double DDPG::selectAction(const State * s, Action * a)
{
	vector<double>& policyOutput = m_pActorTargetNetwork->evaluate(s, a);

	//copy the output of the policy to the action
	m_actorPolicy->vectorToAction(policyOutput, 0, a);

	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//No need to add noise
		return 1.0;
	}

	size_t noiseSignalIndex;
	double noise;

	for (size_t i = 0; i < m_actorPolicy->getUsedActionVariables().size(); i++)
	{
		//if there are less noise signals than output action variables, just use the last one
		noiseSignalIndex = std::min(i, m_noiseSignals.size() - 1);

		noise = m_noiseSignals[i]->getSample();
		const char* variableName = m_actorPolicy->getUsedActionVariables()[i].c_str();
		a->set(variableName, a->get(variableName) + noise);
	}

	return 1.0;
}


/// <summary>
/// Updates the critic and actor using the DDPG algorithm
/// </summary>
/// <param name="s">Initial state</param>
/// <param name="a">Action</param>
/// <param name="s_p">Resultant state</param>
/// <param name="r">Reward</param>
double DDPG::update(const State * s, const Action * a, const State * s_p, double r, double behaviorProb)
{
	if (!m_pActorMinibatch->isFull() && !m_pCriticMinibatch->isFull())
	{
		m_pActorMinibatch->addTuple(s, a, s_p, r);
		m_pCriticMinibatch->addTuple(s, a, s_p, r);
	}
	else
	{
		updateCritic(s, a, s_p, r);
		updateActor(s, a, s_p, r);

		m_numUpdates++;

		m_pActorMinibatch->clear();
		m_pCriticMinibatch->clear();
	}
	
	return 0.0;
}

void DDPG::updateActor(const State* s, const Action* a, const State* s_p, double r)
{
	SimGod* pSimGod = SimionApp::get()->pSimGod.ptr();

	double gamma = SimionApp::get()->pSimGod->getGamma();

	//get pi(s)
	m_pActorTargetNetwork->evaluate( m_pActorMinibatch->s(), m_pi_s);

	//gradient = critic->gradient(s, pi(s))
	m_pCriticTargetNetwork->gradientWrtAction( m_pCriticMinibatch->s(), m_pi_s, m_gradient);

	//gradient = -gradient
	for (size_t i = 0; i < m_pActorMinibatch->target().size(); i++)
		m_pActorMinibatch->target()[i] = - m_gradient[i];

	m_pActorOnlineNetwork->applyGradient(m_pActorMinibatch->s(), m_pActorMinibatch->target());

	/*if (m_numUpdates % m_targetFunctionUpdateFreq == 0)
	{
		m_pActorTargetNetwork->destroy();
		m_pActorTargetNetwork = (IDeterministicPolicyNetwork*) m_pActorOnlineNetwork->clone();
	}*/

	m_pActorTargetNetwork->softUpdate(m_pActorOnlineNetwork, m_tau.get());
}

void DDPG::updateCritic(const State* s, const Action* a, const State* s_p, double r)
{
	SimGod* pSimGod = SimionApp::get()->pSimGod.ptr();

	double gamma = pSimGod->getGamma();

	//calculate pi(s_p)
	m_pActorTargetNetwork->evaluate(m_pActorMinibatch->s_p(), m_pi_s_p);

	m_pCriticTargetNetwork->evaluate(m_pCriticMinibatch->s_p(), m_pi_s_p, m_Q_pi_s_p);

	//for each tuple in the minibatch
	for (int i = 0; i < m_pCriticMinibatch->size(); i++)
	{
		//calculate targetvalue= r + gamma*Q(s_p,a)
		m_pCriticMinibatch->target()[i] = m_pCriticMinibatch->r()[i] + gamma * m_Q_pi_s_p[i];
	}

	//update the network finally
	m_pCriticOnlineNetwork->train(m_pCriticMinibatch, m_pCriticMinibatch->target(), m_criticQFunction->getLearningRate());

	//move the target weights toward the online weights
	m_pCriticTargetNetwork->softUpdate(m_pCriticOnlineNetwork, m_tau.get());
	/*
	if (m_numUpdates % m_targetFunctionUpdateFreq == 0)
	{
		m_pCriticTargetNetwork->destroy();
		m_pCriticTargetNetwork = (IContinuousQFunctionNetwork*) m_pCriticOnlineNetwork->clone();
	}*/
}

#endif
