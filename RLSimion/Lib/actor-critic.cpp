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

#include "actor-critic.h"
#include "config.h"
#include "actor.h"
#include "critic.h"
#include "app.h"
#include "simgod.h"
#include "logger.h"

#define MIN_PROBABILITY 0.00001

ActorCritic::ActorCritic(ConfigNode* pConfigNode)
{
	m_td = 0.0;
	SimionApp::get()->pLogger->addVarToStats<double>("TD-error", "TD-error", m_td);

	m_pActor = CHILD_OBJECT<Actor>(pConfigNode, "Actor", "The actor");
	m_pCritic = CHILD_OBJECT_FACTORY<ICritic>(pConfigNode, "Critic", "The critic");
}

/// <summary>
/// Objects that implement both an actor and a critic call the actor's selectAction() method
/// </summary>
/// <param name="s">Initial state</param>
/// <param name="a">Output action</param>
/// <returns>Probability by which the action was taken. Should be ignored</returns>
double ActorCritic::selectAction(const State *s, Action *a)
{
	return m_pActor->selectAction(s, a);
}


/// <summary>
/// Encapsulates the basic Actor-Critic update: the critic calculates the TD error and the actor updates its policy accordingly
/// </summary>
/// <param name="s">Initial state</param>
/// <param name="a">Action</param>
/// <param name="s_p">Resultant state</param>
/// <param name="r">Reward</param>
/// <param name="behaviorProb">Probability by which the actor selected the action. Should be ignored</param>
/// <returns>The Temporal-Difference error</returns>
double ActorCritic::update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb)
{
	//sample importance weigthing: rho= pi(a_t|x_t) / mu(a_t|x_t)
	//where mu is the behavior from which the sample is drawn

	double sampleWeight = 1.0;
	double policyProb;
	if (SimionApp::get()->pSimGod->useSampleImportanceWeights())
	{
		policyProb = m_pActor->getActionProbability(s,a,false);
		sampleWeight = policyProb;// std::min(10.0, std::max(MIN_PROBABILITY, policyProb / behaviorProb));
	}

	m_td= m_pCritic->update(s, a, s_p, r,sampleWeight);

	m_pActor->update(s, a, s_p, r, m_td);

	return m_td;
}
