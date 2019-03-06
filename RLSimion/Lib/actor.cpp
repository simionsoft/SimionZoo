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

#include "actor.h"
#include "noise.h"
#include "controller.h"
#include "actor.h"
#include "policy.h"
#include "../Common/named-var-set.h"
#include "config.h"
#include "policy-learner.h"
#include "app.h"
#include "vfa.h"
#include "featuremap.h"
#include "simgod.h"
#include "worlds/world.h"
#include "logger.h"
#include <algorithm>

Actor::Actor(ConfigNode* pConfigNode): DeferredLoad(10)
{
	m_policyLearners= MULTI_VALUE_FACTORY<PolicyLearner>(pConfigNode, "Output", "The outputs of the actor. One for each output dimension");
	m_pInitController= CHILD_OBJECT_FACTORY<Controller>(pConfigNode, "Base-Controller", "The base controller used to initialize the weights of the actor", true);
}

Actor::~Actor() {}

void Actor::deferredLoadStep()
{
	size_t numWeights;
	IMemBuffer *pWeights;
	State* s= SimionApp::get()->pWorld->getDynamicModel()->getStateInstance();
	Action* a= SimionApp::get()->pWorld->getDynamicModel()->getActionInstance();
	
	if (m_pInitController.sharedPtr())
	{
		size_t numActionDims = std::min((size_t) m_pInitController->getNumOutputs(), m_policyLearners.size());
		Logger::logMessage(MessageType::Info, "Initializing the policy weights using the base controller");
		//initialize the weights using the controller's output at each center point in state space
		for (size_t actionIndex = 0; actionIndex < numActionDims; actionIndex++)
		{
			for (size_t actorActionIndex = 0; actorActionIndex < m_policyLearners.size(); actorActionIndex++)
			{
				if (!strcmp(m_pInitController->getOutputAction(actionIndex), m_policyLearners[actorActionIndex]->getPolicy()->getOutputAction()))
				{
					//controller's output action index and actor's match, so we use it to initialize
					numWeights = m_policyLearners[actorActionIndex]->getPolicy()->getDetPolicyStateVFA()->getNumWeights();
					pWeights = m_policyLearners[actorActionIndex]->getPolicy()->getDetPolicyStateVFA()->getWeights();
					for (size_t i = 0; i < numWeights; i++)
					{
						m_policyLearners[actorActionIndex]->getPolicy()->getDetPolicyStateVFA()->getStateFeatureMap()->getFeatureStateAction(i, s, a);
						m_pInitController->selectAction(s, a);
						(*pWeights)[i] = a->get(m_pInitController->getOutputAction(actionIndex));
					}
				}
			}
		}
		Logger::logMessage(MessageType::Info, "Initialization done");
	}

	delete s;
	delete a;
}

/// <summary>
/// Iterates over the actor's policy learners so that every one determines its output action
/// </summary>
/// <param name="s">Input initial state</param>
/// <param name="a">Output action</param>
/// <returns>The probability by which the action was selected. Should be ignored</returns>
double Actor::selectAction(const State *s, Action *a)
{
	double prob = 1.0;
	for (unsigned int i = 0; i<m_policyLearners.size(); i++)
	{
		//each uni-dimensional policy sets its own action's value
		prob*= m_policyLearners[i]->getPolicy()->selectAction(s, a);
	}
	return prob;
}

double Actor::getActionProbability(const State *s, const Action *a, bool bStochastic)
{
	double prob = 1.0;
	double ret;

	for (unsigned int i = 0; i<m_policyLearners.size(); i++)
	{
		//each uni-dimensional policy sets its own action's value
		ret = m_policyLearners[i]->getPolicy()->getProbability(s, a, bStochastic);

		if (SimionApp::get()->pSimGod->useSampleImportanceWeights())
			prob *= ret;
	}
	return prob;
}

/// <summary>
/// Iterates over all the actor's policy learners so that every one learns from an experience tuple {s,a,s_p,r,td}
/// </summary>
/// <param name="s">Initial state</param>
/// <param name="a">Action</param>
/// <param name="s_p">Resultant state</param>
/// <param name="r">Reward</param>
/// <param name="td">Temporal-Difference error calculated by the critic</param>
void Actor::update(const State* s, const Action* a, const State* s_p, double r, double td)
{
	for (unsigned int i = 0; i<m_policyLearners.size(); i++)
	{
		//each uni-dimensional policy sets its own action's value
		m_policyLearners[i]->update(s, a, s_p, r, td);
	}
}