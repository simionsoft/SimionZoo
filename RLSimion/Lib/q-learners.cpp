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

#include "q-learners.h"
#include "../Common/named-var-set.h"
#include "vfa.h"
#include "config.h"
#include "parameters-numeric.h"
#include "noise.h"
#include "etraces.h"
#include "worlds/world.h"
#include "app.h"
#include "featuremap.h"
#include <algorithm>
#include <assert.h>
#include "simgod.h"
#include "experiment.h"

#include <math.h>

///////////////////////////////////////
//Q-function-based POLICIES
std::shared_ptr<QPolicy> QPolicy::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<QPolicy>(pConfigNode,"Policy", "The exploration policy used to learn",
	{
		{"Epsilon-Greedy",CHOICE_ELEMENT_NEW<QEGreedyPolicy>},
		{"Soft-Max",CHOICE_ELEMENT_NEW<QSoftMaxPolicy>},
		{"Greedy-Q-Plus-Noise",CHOICE_ELEMENT_NEW<GreedyQPlusNoisePolicy>}
	});
}

//Epsilon-greedy
QEGreedyPolicy::QEGreedyPolicy(ConfigNode* pConfigNode)
{
	m_pEpsilon= CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "Epsilon", "The epsilon parameter that balances exploitation and exploration");
}

QEGreedyPolicy::~QEGreedyPolicy()
{
}

/// <summary>
/// Implements an epsilon-greedy action selection policy, selecting the action with the maximum Q(s,a) value
/// </summary>
/// <param name="pQFunction">The Q-function</param>
/// <param name="s">Current state</param>
/// <param name="a">Output action</param>
/// <returns>Probability by which the action was selected</returns>
double QEGreedyPolicy::selectAction(LinearStateActionVFA* pQFunction, const State* s, Action* a)
{
	double epsilon = m_pEpsilon->get();
	double randomValue = getRandomValue();

	if (SimionApp::get()->pExperiment->isEvaluationEpisode() || randomValue >= epsilon)
	{
		pQFunction->argMax(s, a);
		return epsilon;
	}
	else
	{
		size_t numActionWeights= pQFunction->getNumActionWeights();
		size_t randomActionWeight = rand() % numActionWeights;
		pQFunction->getActionFeatureMap()->getFeatureStateAction(randomActionWeight, (State*) s, a);
		return 1.0 / (double) numActionWeights;
	}
}

//Soft-Max
QSoftMaxPolicy::QSoftMaxPolicy(ConfigNode* pConfigNode)
{
	m_pProbabilities = 0;
	m_pTau= CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode,"Tau", "Temperature parameter");
}

QSoftMaxPolicy::~QSoftMaxPolicy()
{
	if (m_pProbabilities) delete[] m_pProbabilities;
}

/// <summary>
/// Implements a Soft-Max action selection policy controlled by temperature parameter Tau
/// </summary>
/// <param name="pQFunction">The Q-function</param>
/// <param name="s">Current state</param>
/// <param name="a">Output action</param>
/// <returns>Probability by which the action was selected</returns>
double QSoftMaxPolicy::selectAction(LinearStateActionVFA* pQFunction, const State* s, Action* a)
{
	if (SimionApp::get()->pExperiment->isEvaluationEpisode() || m_pTau->get() == 0.0)
	{
		pQFunction->argMax(s, a);
		return 1.0;
	}

	size_t numActionWeights = pQFunction->getNumActionWeights();
	//allocate the probability array if we have to
	//we don't use DeferredLoadStep, because we need pQFunction
	if (!m_pProbabilities)
	{
		if (numActionWeights > 0)
			m_pProbabilities = new double[numActionWeights];
	}
	double inv_tau = 1.0/std::max(0.000001,m_pTau->get());
	pQFunction->getActionValues(s, m_pProbabilities);
	double sum = 0.0;
	size_t i;
	for (i = 0; i < numActionWeights; i++)
	{
		m_pProbabilities[i] = exp(m_pProbabilities[i] * inv_tau);
		sum += m_pProbabilities[i];
	}

	double randomValue = getRandomValue() * sum;
	double searchSum = m_pProbabilities[0];
	double actionProbability = m_pProbabilities[0];
	i= 1;
	while (randomValue>searchSum && i<numActionWeights - 1)
	{
		searchSum += m_pProbabilities[i];
		actionProbability = m_pProbabilities[i];
		i++;
	}
	pQFunction->getActionFeatureMap()->getFeatureStateAction((unsigned int) (i-1), (State*)s, a);
	return actionProbability;
	assert(i < numActionWeights);
}

//GreedyQPlusNoisePolicy
GreedyQPlusNoisePolicy::GreedyQPlusNoisePolicy(ConfigNode* pConfigNode)
{
	m_noise = MULTI_VALUE_FACTORY<Noise>(pConfigNode, "Noise", "Noise signal added to the typical greedy action selection. The number of noise signals should match the number of actions in the action feature amp");
}

GreedyQPlusNoisePolicy::~GreedyQPlusNoisePolicy()
{}


/// <summary>
/// Implements an action selection policy that adds noise to the greedily selected action
/// </summary>
/// <param name="pQFunction">The Q-function</param>
/// <param name="s">Current state</param>
/// <param name="a">Output action</param>
/// <returns>Probability by which the action was selected</returns>
double GreedyQPlusNoisePolicy::selectAction(LinearStateActionVFA* pQFunction, const State* s, Action* a)
{
	if (m_noise.size() != pQFunction->getInputActionVariables().size())
		throw ("GreedyQPlusNoisePolicy: the number of noise signals must match the number of actions in the action feature map");

	if (!SimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//greedy action selection
		pQFunction->argMax(s, a);

		size_t action = 0;
		for (string outputAction : pQFunction->getInputActionVariables())
		{
			a->set(outputAction.c_str(), 0.5*a->get(outputAction.c_str()) + 0.5*m_noise[action]->getSample() );
			action++;
		}
	}
	else
	{
		//greedy action selection
		pQFunction->argMax(s, a);
	}
	return 1.0;
}

///////////////////////////////////
//Q-Learning

QLearningCritic::QLearningCritic(ConfigNode* pConfigNode)
{
	m_pQFunction = CHILD_OBJECT<LinearStateActionVFA>(pConfigNode, "Q-Function", "The parameterization of the Q-Function");
	m_pQFunction->setCanUseDeferredUpdates(true);
	SimionApp::get()->registerStateActionFunction("Q", m_pQFunction.ptr());
	
	m_eTraces = CHILD_OBJECT<ETraces>(pConfigNode, "E-Traces", "E-Traces", true);
	m_eTraces->setName("Q-Learning/traces");
	m_pAlpha = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "Alpha", "The learning gain [0-1]");

	m_pAux = new FeatureList("QLearning/aux");
}

QLearningCritic::~QLearningCritic()
{
	delete m_pAux;
}

/// <summary>
/// Updates the estimate of the Q-function using the Q-Learning update rule with tuple {s,a,s_p,r}
/// </summary>
/// <param name="s">Initial state</param>
/// <param name="a">Action</param>
/// <param name="s_p">Resultant state</param>
/// <param name="r">Reward</param>
/// <returns>The Temporal-Difference error</returns>
double QLearningCritic::update(const State *s, const Action *a, const State *s_p, double r, double probability)
{
	//https://webdocs.cs.ualberta.ca/~sutton/book/ebook/node78.html
	m_eTraces->update();

	double gamma = SimionApp::get()->pSimGod->getGamma();
	m_pQFunction->getFeatures(s, a, m_pAux);
	m_eTraces->addFeatureList(m_pAux, gamma);

	double s_p_value = gamma*m_pQFunction->max(s_p, true);
	double s_value = m_pQFunction->get(m_pAux, false); //we use the live weights instead of the frozen ones
	double td = r + s_p_value - s_value;

	m_pQFunction->add(m_eTraces.ptr(), td*m_pAlpha->get());

	if (m_bUseVFunctionAsBaseline)
		return r + s_p_value - m_pQFunction->max(s, true);
	else return td;
}

QLearning::QLearning(ConfigNode* pConfigNode): QLearningCritic(pConfigNode), Simion(pConfigNode)
{
	m_pQPolicy= CHILD_OBJECT_FACTORY<QPolicy>(pConfigNode, "Policy", "The policy to be followed");
	m_bUseVFunctionAsBaseline = false;
}

QLearning::~QLearning()
{

}

double QLearning::update(const State *s, const Action *a, const State *s_p, double r, double probability)
{
	return QLearningCritic::update(s, a, s_p, r, probability);
}


double QLearning::selectAction(const State *s, Action *a)
{
	return m_pQPolicy->selectAction(m_pQFunction.ptr(), s, a);
}

///////////////////////////////////////////////////
//Q-Learning
DoubleQLearning::DoubleQLearning(ConfigNode* pConfigNode) : QLearning(pConfigNode)
{
	//no need to parameterize the second Q-function (Q_b), just clone the original q-function (Q_a)
	m_pQFunction2 = new LinearStateActionVFA(m_pQFunction.ptr());
}

DoubleQLearning::~DoubleQLearning()
{
	delete m_pQFunction2;
}

/// <summary>
/// Updates the estimate of the Q-function using the Double Q-Learning update rule with tuple {s,a,s_p,r}.
/// The main difference with respect to Q-function is that it uses two different sets of weights for the function,
/// updating a random set of weights toward the other set of weights. Should offer better stability than regular
/// Q-Learning
/// </summary>
/// <param name="s">Initial state</param>
/// <param name="a">Action</param>
/// <param name="s_p">Resultant state</param>
/// <param name="r">Reward</param>
/// <returns>The Temporal-Difference error</returns>
double DoubleQLearning::update(const State *s, const Action *a, const State *s_p, double r, double probability)
{
	double gamma= SimionApp::get()->pSimGod->getGamma();
	m_pQFunction->getFeatures(s, a, m_pAux);

	//Randomly select the target function
	LinearStateActionVFA *pQ_a, *pQ_b;
	if (getRandomValue()<0.5)
	{
		pQ_a = m_pQFunction.ptr();
		pQ_b = m_pQFunction2;
	}
	else
	{
		pQ_b = m_pQFunction.ptr();
		pQ_a = m_pQFunction2;
	}

	double td = r + gamma*pQ_b->max(s_p) - pQ_a->get(s, a);

	pQ_a->add(m_pAux, td * m_pAlpha->get());

	return td;
}

/////////////////////////////////////////////////
//SARSA
SARSA::SARSA(ConfigNode* pConfigNode) : QLearning(pConfigNode)
{
	m_nextAProbability = 0.0;
	m_bNextActionSelected = false;
	m_nextA = SimionApp::get()->pWorld->getDynamicModel()->getActionInstance();
}

SARSA::~SARSA()
{
	delete m_nextA;
}


/// <summary>
/// implements SARSA On-policy action selection algorithm
/// </summary>
/// <param name="s">Initial state</param>
/// <param name="a">Output action</param>
double SARSA::selectAction(const State *s, Action *a)
{
	if (m_bNextActionSelected)
	{
		a->copy(m_nextA);
		m_bNextActionSelected = false;
		return m_nextAProbability;
	}
	else
	{
		return m_pQPolicy->selectAction(m_pQFunction.ptr(), s, a);
	}
}

/// <summary>
/// Updates the estimate of the Q-function using the SARSA update rule with tuple {s,a,s_p,r}
/// </summary>
/// <param name="s">Initial state</param>
/// <param name="a">Action</param>
/// <param name="s_p">Resultant state</param>
/// <param name="r">Reward</param>
/// <returns>The Temporal-Difference error</returns>
double SARSA::update(const State* s, const Action* a, const State* s_p, double r, double probability)
{
	//https://webdocs.cs.ualberta.ca/~sutton/book/ebook/node77.html
	m_eTraces->update();

	//select a_t+1
	m_nextAProbability= m_pQPolicy->selectAction(m_pQFunction.ptr(), s_p, m_nextA);
	m_bNextActionSelected = true;

	double gamma= SimionApp::get()->pSimGod->getGamma();
	m_pQFunction->getFeatures(s, a, m_pAux);
	m_eTraces->addFeatureList(m_pAux, gamma);

	double td = r + gamma*m_pQFunction->get(s_p,m_nextA) - m_pQFunction->get(s, a);
	m_pQFunction->add(m_eTraces.ptr(), td*m_pAlpha->get());
	return td;
}