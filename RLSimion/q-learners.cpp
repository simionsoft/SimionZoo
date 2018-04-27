#include "q-learners.h"
#include "named-var-set.h"
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

///////////////////////////////////////
//Q-function-based POLICIES
std::shared_ptr<QPolicy> QPolicy::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<QPolicy>(pConfigNode,"Policy", "The exploration policy used to learn",
	{
		{"Epsilon-Greedy",CHOICE_ELEMENT_NEW<QEGreedyPolicy>},
		{"Soft-Max",CHOICE_ELEMENT_NEW<QSoftMaxPolicy>}
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

double QEGreedyPolicy::selectAction(LinearStateActionVFA* pQFunction, const State* s, Action* a)
{
	double epsilon = m_pEpsilon->get();
	double randomValue = getRandomValue();

	if (randomValue >= epsilon)
	{
		pQFunction->argMax(s, a);
		return epsilon;
	}
	else
	{
		size_t numActionWeights= pQFunction->getNumActionWeights();
		size_t randomActionWeight = rand() % numActionWeights;
		pQFunction->getActionFeatureMap()->getFeatureAction((unsigned int)randomActionWeight, a);
		return 1.0 / numActionWeights;
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

double QSoftMaxPolicy::selectAction(LinearStateActionVFA* pQFunction, const State* s, Action* a)
{
	if (m_pTau->get() == 0.0)
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
	pQFunction->getActionFeatureMap()->getFeatureAction((unsigned int) (i-1), a);
	return actionProbability;
	assert(i < numActionWeights);
}

///////////////////////////////////
//Q-Learning

QLearningCritic::QLearningCritic(ConfigNode* pConfigNode)
{
	m_pQFunction = CHILD_OBJECT<LinearStateActionVFA>(pConfigNode, "Q-Function", "The parameterization of the Q-Function");
	m_pQFunction->setCanUseDeferredUpdates(true);
	SimionApp::get()->registerStateActionFunction("Q(s,a)", m_pQFunction.ptr());
	
	m_eTraces = CHILD_OBJECT<ETraces>(pConfigNode, "E-Traces", "E-Traces", true);
	m_eTraces->setName("Q-Learning/traces");
	m_pAlpha = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "Alpha", "The learning gain [0-1]");

	m_pAux = new FeatureList("QLearning/aux");
}

QLearningCritic::~QLearningCritic()
{
	delete m_pAux;
}

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

QLearning::QLearning(ConfigNode* pConfigNode): QLearningCritic(pConfigNode)
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

	pQ_a->add(m_pAux, td);

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