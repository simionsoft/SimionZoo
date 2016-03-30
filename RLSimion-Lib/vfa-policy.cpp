#include "stdafx.h"
#include "named-var-set.h"
#include "noise.h"
#include "vfa.h"
#include "policy-learner.h"
#include "parameters.h"
#include "globals.h"
#include "world.h"
#include "features.h"
#include "policy.h"

CLASS_FACTORY(CDeterministicPolicy)
{
	CHOICE("Policy","The policy type");

	CHOICE_ELEMENT("Deterministic-Policy-Gaussian-Noise", CDeterministicPolicyGaussianNoise,"A deterministic policy pi(s) to which some noise is added");
//	CHOICE_ELEMENT("Stochastic-Policy-Gaussian-Noise", CStochasticPolicyGaussianNoise,"An stochastic policy pi(s)= N(pi_mean(s),pi_variance(s))");

	END_CHOICE();
	END_CLASS();
	return 0;
}

CLASS_CONSTRUCTOR(CDeterministicPolicy)
: CParamObject(pParameters)
{
	
	ACTION_VARIABLE_REF(m_outputActionIndex, "Output-Action","The output action variable");

	END_CLASS();
}

CDeterministicPolicy::~CDeterministicPolicy()
{
}




//CDetPolicyGaussianNoise////////////////////////////////
/////////////////////////////////////////////////////////

CLASS_CONSTRUCTOR(CDeterministicPolicyGaussianNoise)
	: EXTENDS(CDeterministicPolicy, pParameters)
{
	CHILD_CLASS_FACTORY(m_pDeterministicVFA, "Deterministic-Policy-VFA", "The parameterized VFA that approximates the function", false, CLinearStateVFA);
	CHILD_CLASS_FACTORY(m_pExpNoise,"Exploration-Noise","Parameters of the noise used as exploration",false,CNoise);

	CAction* pActionDescriptor = CWorld::getDynamicModel()->getActionDescriptor();
	m_pDeterministicVFA->saturateOutput(pActionDescriptor->getMin(m_outputActionIndex), pActionDescriptor->getMax(m_outputActionIndex));
	END_CLASS();
}

CDeterministicPolicyGaussianNoise::~CDeterministicPolicyGaussianNoise()
{
	delete m_pDeterministicVFA;
	delete m_pExpNoise;
}

void CDeterministicPolicyGaussianNoise::getNaturalGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient)
{
	//0. Grad_u pi(a|s)/pi(a|s) = (a - pi(s)) * phi(s) / sigma*2
	m_pDeterministicVFA->getFeatures(s, pOutGradient);

	//TXAPUZAAAA^2!!!
	double sigma = ((CGaussianNoise*)m_pExpNoise)->getSigma();

	double noise = a->getValue(m_outputActionIndex) - m_pDeterministicVFA->getValue((const CFeatureList*)pOutGradient);

	double unscaled_noise = ((CGaussianNoise*)m_pExpNoise)->unscale(noise);
	double factor = unscaled_noise / (sigma*sigma);
}

void CDeterministicPolicyGaussianNoise::selectAction(const CState *s, CAction *a)
{
	double noise;
	double output;

	noise = m_pExpNoise->getValue();

	output = m_pDeterministicVFA->getValue(s);

	a->setValue(m_outputActionIndex, output + noise);
}

void CDeterministicPolicyGaussianNoise::getFeatures(const CState* state, CFeatureList* outFeatureList)
{
	m_pDeterministicVFA->getFeatures(state, outFeatureList);
}
void CDeterministicPolicyGaussianNoise::addFeatures(const CFeatureList* pFeatureList, double factor)
{
	m_pDeterministicVFA->add(pFeatureList, factor);
}
double CDeterministicPolicyGaussianNoise::getValue(const CFeatureList* pFeatureList)
{
	return m_pDeterministicVFA->getValue(pFeatureList);
}

//CStoPolicyGaussianNoise//////////////////////////
////////////////////////////////////////////////
/*
CLASS_CONSTRUCTOR(CStochasticPolicyGaussianNoise)
	: EXTENDS(CDeterministicPolicy, pParameters)
{
	CHILD_CLASS_FACTORY(m_pMeanVFA, "Mean-VFA", "The parameterized VFA that approximates the function", false, CLinearStateVFA);
	CHILD_CLASS_FACTORY(m_pSigmaVFA, "Sigma-VFA", "The parameterized VFA that approximates variance(s)", false, CLinearStateVFA);

	m_pAux = new CFeatureList("Sto-Policy/aux");
	END_CLASS();
}

CStochasticPolicyGaussianNoise::~CStochasticPolicyGaussianNoise()
{
	delete m_pMeanVFA;
	delete m_pSigmaVFA;
	delete m_pAux;
}

void CStochasticPolicyGaussianNoise::selectAction(const CState *s, CAction *a)
{
	double sigma;
	double mean;
	double output;

	sigma = m_pSigmaVFA->getValue(s);

	mean = m_pMeanVFA->getValue(s);

	output = getNormalDistributionSample(mean, sigma);

	a->setValue(m_outputActionIndex, output);
}

//returns the factor by which the state features have to be multiplied to get the policy gradient
void CStochasticPolicyGaussianNoise::getNaturalGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient)
{

}*/