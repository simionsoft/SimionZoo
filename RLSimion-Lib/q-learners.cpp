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

void CQEGreedyPolicy::selectAction(CLinearStateActionVFA* pQFunction, const CState* s, CAction* a)
{
	double epsilon = m_pEpsilon->getValue();
	double randomValue = getRandomValue();

	if (randomValue < epsilon)
		pQFunction->argMax(s, a);
	else
	{
		unsigned int numActionWeights= pQFunction->getNumActionWeights();
		unsigned int randomActionWeight = rand() % numActionWeights;
		pQFunction->getActionFeatureMap()->getFeatureAction(randomActionWeight, a);
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

void CQSoftMaxPolicy::selectAction(CLinearStateActionVFA* pQFunction, const CState* s, CAction* a)
{
	if (m_pTau->getValue() == 0.0)
	{
		pQFunction->argMax(s, a);
		return;
	}

	unsigned int numActionWeights = pQFunction->getNumActionWeights();
	//allocate the probability array if we have to
	if (!m_pProbabilities)
	{
		if (numActionWeights > 0)
			m_pProbabilities = new double[numActionWeights];
	}
	double inv_tau = 1.0/std::max(0.000001,m_pTau->getValue());
	pQFunction->getActionValues(s, m_pProbabilities);
	double sum = 0.0;
	unsigned int i;
	for (i = 0; i < numActionWeights; i++)
	{
		m_pProbabilities[i] = exp(m_pProbabilities[i] * inv_tau);
		sum += m_pProbabilities[i];
	}
	sum = 1.0 / sum;
	double randomValue = getRandomValue();
	double searchSum = m_pProbabilities[0] * sum;
	i= 1;
	while (randomValue>searchSum && i<numActionWeights - 1)
	{
		searchSum += m_pProbabilities[i] * sum;
		i++;
	}
	pQFunction->getActionFeatureMap()->getFeatureAction(i, a);
	assert(i < numActionWeights);
}

///////////////////////////////////
//Q-Learning

CQLearning::CQLearning(CConfigNode* pConfigNode)
{
	m_pQFunction= CHILD_OBJECT<CLinearStateActionVFA>(pConfigNode, "Q-Function", "The parameterization of the Q-Function");
	m_pQPolicy= CHILD_OBJECT_FACTORY<CQPolicy>(pConfigNode, "Policy", "The policy to be followed");
	m_eTraces= CHILD_OBJECT<CETraces>(pConfigNode, "E-Traces", "E-Traces");
	m_eTraces->setName("Q-Learning/traces");
	m_pGamma= CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "Gamma", "The gamma parameter of the MDP");
	m_pAlpha = CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "Alpha", "The learning gain [0-1]");
	m_pAux = new CFeatureList("QLearning/aux");
	//CHILD_CLASS(m_pQFunction, "Q-Function", "The parameterization of the Q-Function", false, CLinearStateActionVFA);
	//CHILD_CLASS_FACTORY(m_pQPolicy, "Policy", "The policy to be followed", false, CQPolicy);
	//CHILD_CLASS(m_eTraces, "E-Traces", "E-Traces", true, CETraces, "Q-Learning/traces");
	//NUMERIC_VALUE(m_pGamma, "Gamma", "The gamma parameter of the MDP");
	//NUMERIC_VALUE(m_pAlpha, "Alpha", "The learning gain [0-1]");
	//m_pAux = new CFeatureList("QLearning/aux");
	//END_CLASS();
}
CQLearning::~CQLearning()
{
	delete m_pAux;
}

void CQLearning::updateValue(const CState *s, const CAction *a, const CState *s_p, double r)
{
	//https://webdocs.cs.ualberta.ca/~sutton/book/ebook/node78.html
	m_eTraces->update();

	m_pQFunction->getFeatures(s, a, m_pAux);
	m_eTraces->addFeatureList(m_pAux, m_pGamma->getValue());

	double td = r + m_pGamma->getValue()*m_pQFunction->max(s_p) - m_pQFunction->getValue(s,a);

	m_pQFunction->add(m_eTraces.ptr(), td*m_pAlpha->getValue());
}

void CQLearning::selectAction(const CState *s, CAction *a)
{
	m_pQPolicy->selectAction(m_pQFunction.ptr(), s, a);
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

void CDoubleQLearning::updateValue(const CState *s, const CAction *a, const CState *s_p, double r)
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

	m_pQFunction->getFeatures(s, a, m_pAux);
	m_eTraces->addFeatureList(m_pAux, m_pGamma->getValue());

	double td = r + m_pGamma->getValue()*m_pQFunction->max(s_p) - m_pTargetQFunction->getValue(s, a);

	m_pQFunction->add(m_eTraces.ptr(), td*m_pAlpha->getValue());
}

/////////////////////////////////////////////////
//SARSA
CSARSA::CSARSA(CConfigNode* pConfigNode) : CQLearning(pConfigNode)
{
	m_bNextActionSelected = false;
	m_nextA = CSimionApp::get()->pWorld->getDynamicModel()->getActionInstance();
}

CSARSA::~CSARSA()
{
	delete m_nextA;
}

void CSARSA::selectAction(const CState *s, CAction *a)
{
	if (m_bNextActionSelected)
	{
		a->copy(m_nextA);
		m_bNextActionSelected = false;
	}
	else
	{
		m_pQPolicy->selectAction(m_pQFunction.ptr(), s, a);
	}
}

void CSARSA::updateValue(const CState* s, const CAction* a, const CState* s_p, double r)
{
	//https://webdocs.cs.ualberta.ca/~sutton/book/ebook/node77.html
	m_eTraces->update();

	//select a_t+1
	m_pQPolicy->selectAction(m_pQFunction.ptr(), s_p, m_nextA);
	m_bNextActionSelected = true;

	m_pQFunction->getFeatures(s, a, m_pAux);
	m_eTraces->addFeatureList(m_pAux, m_pGamma->getValue());

	double td = r + m_pGamma->getValue()*m_pQFunction->getValue(s_p,m_nextA) - m_pQFunction->getValue(s, a);
	m_pQFunction->add(m_eTraces.ptr(), td*m_pAlpha->getValue());
}