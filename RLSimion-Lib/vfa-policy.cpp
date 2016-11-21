#include "stdafx.h"
#include "named-var-set.h"
#include "noise.h"
#include "vfa.h"
#include "policy-learner.h"
#include "config.h"
#include "world.h"
#include "features.h"
#include "policy.h"

std::shared_ptr<CPolicy> CPolicy::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE<CPolicy>(pConfigNode,"Policy", "The policy type"
		, {
	CHOICE_ELEMENT_NEW(pConfigNode,CDeterministicPolicyGaussianNoise,"Deterministic-Policy-Gaussian-Noise", "A deterministic policy pi(s) to which some noise is added",""),
	CHOICE_ELEMENT_NEW(pConfigNode,CStochasticPolicyGaussianNoise,"Stochastic-Policy-Gaussian-Noise", "An stochastic policy pi(s)= N(pi_mean(s),pi_variance(s))","")
		});
	//CHOICE("Policy","The policy type");

	//CHOICE_ELEMENT("Deterministic-Policy-Gaussian-Noise", CDeterministicPolicyGaussianNoise,"A deterministic policy pi(s) to which some noise is added");
	//CHOICE_ELEMENT("Stochastic-Policy-Gaussian-Noise", CStochasticPolicyGaussianNoise,"An stochastic policy pi(s)= N(pi_mean(s),pi_variance(s))");

	//END_CHOICE();
	//END_CLASS();
	//return 0;
}

CPolicy::CPolicy(CConfigNode* pConfigNode)
{
	
	m_outputActionIndex= ACTION_VARIABLE(pConfigNode,"Output-Action","The output action variable");

}

CPolicy::~CPolicy()
{
}




//CDetPolicyGaussianNoise////////////////////////////////
/////////////////////////////////////////////////////////

CDeterministicPolicyGaussianNoise::CDeterministicPolicyGaussianNoise(CConfigNode* pConfigNode)
	: CPolicy(pConfigNode)
{
	m_pDeterministicVFA= CHILD_OBJECT<CLinearStateVFA>(pConfigNode, "Deterministic-Policy-VFA"
		, "The parameterized VFA that approximates the function");
	m_pExpNoise= CHILD_OBJECT_FACTORY<CNoise>(pConfigNode,"Exploration-Noise"
		,"Parameters of the noise used as exploration");

	CDescriptor& pActionDescriptor = CWorld::getDynamicModel()->getActionDescriptor();
	m_pDeterministicVFA->saturateOutput(pActionDescriptor[m_outputActionIndex.get()].getMin()
		, pActionDescriptor[m_outputActionIndex.get()].getMax());
}

CDeterministicPolicyGaussianNoise::~CDeterministicPolicyGaussianNoise()
{
}

void CDeterministicPolicyGaussianNoise::getNaturalGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient)
{
	//0. Grad_u pi(a|s)/pi(a|s) = (a - pi(s)) * phi(s) / sigma*2
	m_pDeterministicVFA->getFeatures(s, pOutGradient);

	double sigma = std::max(0.0000001,m_pExpNoise->getSigma());

	double noise = a->getValue(m_outputActionIndex.get()) 
		- m_pDeterministicVFA->getValue((const CFeatureList*)pOutGradient);

	double unscaled_noise = m_pExpNoise->unscale(noise);
	double factor = unscaled_noise / (sigma*sigma);

	pOutGradient->mult(factor);
}

void CDeterministicPolicyGaussianNoise::selectAction(const CState *s, CAction *a)
{
	double noise;
	double output;

	noise = m_pExpNoise->getValue();

	output = m_pDeterministicVFA->getValue(s);

	a->setValue(m_outputActionIndex.get(), output + noise);
}

void CDeterministicPolicyGaussianNoise::getFeatures(const CState* state, CFeatureList* outFeatureList)
{
	m_pDeterministicVFA->getFeatures(state, outFeatureList);
}
void CDeterministicPolicyGaussianNoise::addFeatures(const CFeatureList* pFeatureList, double factor)
{
	m_pDeterministicVFA->add(pFeatureList, factor);
}
double CDeterministicPolicyGaussianNoise::getDeterministicOutput(const CFeatureList* pFeatureList)
{
	return m_pDeterministicVFA->getValue(pFeatureList);
}

//CStoPolicyGaussianNoise//////////////////////////
////////////////////////////////////////////////

CStochasticPolicyGaussianNoise::CStochasticPolicyGaussianNoise(CConfigNode* pConfigNode)
	: CPolicy(pConfigNode)
{
	m_pMeanVFA= CHILD_OBJECT<CLinearStateVFA>(pConfigNode, "Mean-VFA", "The parameterized VFA that approximates the function");
	m_pSigmaVFA= CHILD_OBJECT<CLinearStateVFA>(pConfigNode, "Sigma-VFA", "The parameterized VFA that approximates variance(s)");
	m_pSigmaVFA->saturateOutput(0.0, 1.0);
	m_pSigmaVFA->setIndexOffset(m_pMeanVFA->getNumWeights());
	m_pMeanFeatures = new CFeatureList("Sto-Policy/mean-features");
	m_pSigmaFeatures = new CFeatureList("Sto-Policy/sigma-features");
}

CStochasticPolicyGaussianNoise::~CStochasticPolicyGaussianNoise()
{
	delete m_pMeanFeatures;
	delete m_pSigmaFeatures;
}

void CStochasticPolicyGaussianNoise::selectAction(const CState *s, CAction *a)
{
	double sigma;
	double mean;
	double output;

	sigma = m_pSigmaVFA->getValue(s);

	mean = m_pMeanVFA->getValue(s);

	output = getNormalDistributionSample(mean, sigma);

	a->setValue(m_outputActionIndex.get(), output);
}


void CStochasticPolicyGaussianNoise::getNaturalGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient)
{
	m_pMeanVFA->getFeatures(s, m_pMeanFeatures);
	m_pSigmaVFA->getFeatures(s, m_pSigmaFeatures);
	double sigma = m_pSigmaVFA->getValue(m_pSigmaFeatures);
	double mean = m_pMeanVFA->getValue(m_pSigmaFeatures);

	//a. Grad_u_mean pi(a|s)/pi(a|s) = (a - pi(s)) * phi_mean(s) / sigma*2
	double noise = a->getValue(m_outputActionIndex.get()) - mean;

	double factor = noise / (sigma*sigma);
	pOutGradient->addFeatureList(m_pMeanFeatures,factor);

	//b. Grad_u_sigma pi(a|s)/pi(a|s) = (a - pi(s))^2 -1 * phi_sigma(s) / sigma*2
	factor = (noise*noise - 1.0) / (sigma*sigma);
	pOutGradient->addFeatureList(m_pSigmaFeatures, factor);
}

void CStochasticPolicyGaussianNoise::getFeatures(const CState* state, CFeatureList* outFeatureList)
{
	m_pMeanVFA->getFeatures(state, outFeatureList);
	m_pSigmaVFA->getFeatures(state, m_pSigmaFeatures);
	outFeatureList->addFeatureList(m_pSigmaFeatures);
}
void CStochasticPolicyGaussianNoise::addFeatures(const CFeatureList* pFeatureList, double factor)
{
	m_pMeanVFA->add(pFeatureList, factor);
}
double CStochasticPolicyGaussianNoise::getDeterministicOutput(const CFeatureList* pFeatureList)
{
	return m_pMeanVFA->getValue(pFeatureList);
}