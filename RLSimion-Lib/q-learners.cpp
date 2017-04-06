#include "stdafx.h"
#include "q-learners.h"
#include "named-var-set.h"
#include "vfa.h"
#include "config.h"
#include "parameters-numeric.h"
#include "noise.h"
#include "etraces.h"
#include "world.h"
#include "app.h"
#include "featuremap.h"

///////////////////////////////////////
//Q-function-based POLICIES
std::shared_ptr<CQPolicy> CQPolicy::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE<CQPolicy>(pConfigNode,"Policy", "The exploration policy used to learn",
	{
		{"Epsilon-Greedy",CHOICE_ELEMENT_NEW<CQEGreedyPolicy>},
		{"Soft-Max",CHOICE_ELEMENT_NEW<CQSoftMaxPolicy>}
	});
}

//Epsilon-greedy
CQEGreedyPolicy::CQEGreedyPolicy(CConfigNode* pConfigNode)
{
	m_pEpsilon= CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "Epsilon", "The epsilon parameter that balances exploitation and exploration");
}

CQEGreedyPolicy::~CQEGreedyPolicy()
{
}

double CQEGreedyPolicy::selectAction(CLinearStateActionVFA* pQFunction, const CState* s, CAction* a)
{
	double epsilon = m_pEpsilon->get();
	double randomValue = getRandomValue();

	if (randomValue < epsilon)
	{
		pQFunction->argMax(s, a);
		return epsilon;
	}
	else
	{
		unsigned int numActionWeights= pQFunction->getNumActionWeights();
		unsigned int randomActionWeight = rand() % numActionWeights;
		pQFunction->getActionFeatureMap()->getFeatureAction(randomActionWeight, a);
		return 1.0 / numActionWeights;
	}
}

//Soft-Max
CQSoftMaxPolicy::CQSoftMaxPolicy(CConfigNode* pConfigNode)
{
	m_pProbabilities = 0;
	m_pTau= CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode,"Tau", "Temperature parameter");
}

CQSoftMaxPolicy::~CQSoftMaxPolicy()
{
	if (m_pProbabilities) delete[] m_pProbabilities;
}

double CQSoftMaxPolicy::selectAction(CLinearStateActionVFA* pQFunction, const CState* s, CAction* a)
{
	if (m_pTau->get() == 0.0)
	{
		pQFunction->argMax(s, a);
		return 1.0;
	}

	unsigned int numActionWeights = pQFunction->getNumActionWeights();
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
	unsigned int i;
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
	pQFunction->getActionFeatureMap()->getFeatureAction(i-1, a);
	return actionProbability;
	assert(i < numActionWeights);
}

///////////////////////////////////
//Q-Learning

CQLearning::CQLearning(CConfigNode* pConfigNode)
{
	m_pQFunction= CHILD_OBJECT<CLinearStateActionVFA>(pConfigNode, "Q-Function", "The parameterization of the Q-Function");
	m_pQPolicy= CHILD_OBJECT_FACTORY<CQPolicy>(pConfigNode, "Policy", "The policy to be followed");
	m_eTraces= CHILD_OBJECT<CETraces>(pConfigNode, "E-Traces", "E-Traces",true);
	m_eTraces->setName("Q-Learning/traces");
	m_pAlpha = CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "Alpha", "The learning gain [0-1]");
	m_pAux = new CFeatureList("QLearning/aux");
}
CQLearning::~CQLearning()
{
	delete m_pAux;
}

void CQLearning::update(const CState *s, const CAction *a, const CState *s_p, double r, double probability)
{
	//https://webdocs.cs.ualberta.ca/~sutton/book/ebook/node78.html
	m_eTraces->update();

	double gamma= CSimionApp::get()->pSimGod->getGamma();
	m_pQFunction->getFeatures(s, a, m_pAux);
	m_eTraces->addFeatureList(m_pAux, gamma);

	double td = r + gamma*m_pQFunction->max(s_p) - m_pQFunction->get(s,a);

	m_pQFunction->add(m_eTraces.ptr(), td*m_pAlpha->get());
}

double CQLearning::selectAction(const CState *s, CAction *a)
{
	return m_pQPolicy->selectAction(m_pQFunction.ptr(), s, a);
}

///////////////////////////////////////////////////
//Q-Learning
CDoubleQLearning::CDoubleQLearning(CConfigNode* pConfigNode) : CQLearning(pConfigNode)
{
	//no need to parameterize it, just clone the original q-function
	m_pTargetQFunction= new CLinearStateActionVFA(m_pQFunction.ptr());

	//CONST_INTEGER_VALUE(m_targetUpdateFreq, "Target-Update-Freq", 100, "The number of steps between updates of the target Q-Function");
	m_targetUpdateFreq = INT_PARAM(pConfigNode, "Target-Update-Freq", "The number of steps between updates of the target Q-Function", 100);
	m_numStepsSinceLastTargetUpdate = 0;


}

CDoubleQLearning::~CDoubleQLearning()
{
	delete m_pTargetQFunction;
}

void CDoubleQLearning::update(const CState *s, const CAction *a, const CState *s_p, double r, double probability)
{
	m_eTraces->update();

	//update the target
	if (m_numStepsSinceLastTargetUpdate > m_targetUpdateFreq.get())
	{
		//copy the weights from the online function to the target function
		memcpy_s(m_pTargetQFunction->getWeightPtr(), m_pTargetQFunction->getNumWeights()*sizeof(double)
			, m_pQFunction->getWeightPtr(), m_pQFunction->getNumWeights()*sizeof(double));
		m_numStepsSinceLastTargetUpdate = 0;
	}
	else m_numStepsSinceLastTargetUpdate++;

	double gamma= CSimionApp::get()->pSimGod->getGamma();
	m_pQFunction->getFeatures(s, a, m_pAux);
	m_eTraces->addFeatureList(m_pAux, gamma);

	double td = r + gamma*m_pQFunction->max(s_p) - m_pTargetQFunction->get(s, a);

	m_pQFunction->add(m_eTraces.ptr(), td);
}

/////////////////////////////////////////////////
//SARSA
CSARSA::CSARSA(CConfigNode* pConfigNode) : CQLearning(pConfigNode)
{
	m_nextAProbability = 0.0;
	m_bNextActionSelected = false;
	m_nextA = CSimionApp::get()->pWorld->getDynamicModel()->getActionInstance();
}

CSARSA::~CSARSA()
{
	delete m_nextA;
}

double CSARSA::selectAction(const CState *s, CAction *a)
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

void CSARSA::update(const CState* s, const CAction* a, const CState* s_p, double r, double probability)
{
	//https://webdocs.cs.ualberta.ca/~sutton/book/ebook/node77.html
	m_eTraces->update();

	//select a_t+1
	m_nextAProbability= m_pQPolicy->selectAction(m_pQFunction.ptr(), s_p, m_nextA);
	m_bNextActionSelected = true;

	double gamma= CSimionApp::get()->pSimGod->getGamma();
	m_pQFunction->getFeatures(s, a, m_pAux);
	m_eTraces->addFeatureList(m_pAux, gamma);

	double td = r + gamma*m_pQFunction->get(s_p,m_nextA) - m_pQFunction->get(s, a);
	m_pQFunction->add(m_eTraces.ptr(), td*m_pAlpha->get());
}