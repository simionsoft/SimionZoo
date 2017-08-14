#include "stdafx.h"
#include "named-var-set.h"
#include "noise.h"
#include "vfa.h"
#include "policy-learner.h"
#include "config.h"
#include "worlds/world.h"
#include "features.h"
#include "policy.h"
#include "app-rlsimion.h"
#include "SimGod.h"
#include "featuremap.h"
#include <iostream>

std::shared_ptr<CPolicy> CPolicy::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE<CPolicy>(pConfigNode, "Policy", "The policy type",
	{
		{"Deterministic-Policy-Gaussian-Noise",CHOICE_ELEMENT_NEW<CDeterministicPolicyGaussianNoise>},
		{"Stochastic-Policy-Gaussian-Noise",CHOICE_ELEMENT_NEW<CStochasticPolicyGaussianNoise>}
	});
}

CPolicy::CPolicy(CConfigNode* pConfigNode)
{

	m_outputActionIndex = ACTION_VARIABLE(pConfigNode, "Output-Action", "The output action variable");

}

CPolicy::~CPolicy()
{
}




//CDetPolicyGaussianNoise////////////////////////////////
/////////////////////////////////////////////////////////

CDeterministicPolicyGaussianNoise::CDeterministicPolicyGaussianNoise(CConfigNode* pConfigNode)
	: CPolicy(pConfigNode)
{
	m_pDeterministicVFA = CHILD_OBJECT<CLinearStateVFA>(pConfigNode, "Deterministic-Policy-VFA"
		, "The parameterized VFA that approximates the function");
	m_pExpNoise = CHILD_OBJECT_FACTORY<CNoise>(pConfigNode, "Exploration-Noise"
		, "Parameters of the noise used as exploration");

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

	double sigma = std::max(0.0000001, m_pExpNoise->getVariance());

	double noise = a->get(m_outputActionIndex.get())
		- m_pDeterministicVFA->get((const CFeatureList*)pOutGradient);

	double unscaled_noise = m_pExpNoise->unscale(noise);
	double factor = unscaled_noise / (sigma*sigma);

	pOutGradient->mult(factor);
}

double CDeterministicPolicyGaussianNoise::selectAction(const CState *s, CAction *a)
{
	double noise;

	if (!CSimionApp::get()->pExperiment->isEvaluationEpisode())
		noise = m_pExpNoise->getSample();
	else noise = 0.0;

	double output = m_pDeterministicVFA->get(s);

	a->set(m_outputActionIndex.get(), output + noise);

	if (!CSimionApp::get()->pExperiment->isEvaluationEpisode())
		return m_pExpNoise->getSampleProbability(noise);
	return 1.0;
}

double CDeterministicPolicyGaussianNoise::getOutput(const CState* s)
{
	double noise;

	if (!CSimionApp::get()->pExperiment->isEvaluationEpisode())
		noise = m_pExpNoise->getSample();
	else noise = 0.0;

	double output = m_pDeterministicVFA->get(s);

	return noise + output;
}

double CDeterministicPolicyGaussianNoise::getProbability(const CState* s, const CAction* a, bool bStochastic)
{
	double noise;
	if (CSimionApp::get()->pSimGod->useSampleImportanceWeights())
	{
		noise = a->get(m_outputActionIndex.get()) - m_pDeterministicVFA->get(s);
		return m_pExpNoise->getSampleProbability(noise, !bStochastic);
	}
	return 1.0;
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
	return m_pDeterministicVFA->get(pFeatureList);
}

//CStoPolicyGaussianNoise//////////////////////////
////////////////////////////////////////////////

CStochasticPolicyGaussianNoise::CStochasticPolicyGaussianNoise(CConfigNode* pConfigNode)
	: CPolicy(pConfigNode)
{
	m_pMeanVFA = CHILD_OBJECT<CLinearStateVFA>(pConfigNode, "Mean-VFA", "The parameterized VFA that approximates the function");
	m_pSigmaVFA = CHILD_OBJECT<CLinearStateVFA>(pConfigNode, "Sigma-VFA", "The parameterized VFA that approximates variance(s)");
	m_pSigmaVFA->saturateOutput(0.0, 1.0);
	m_pSigmaVFA->setIndexOffset(m_pMeanVFA->getNumWeights());
	m_pMeanFeatures = new CFeatureList("Sto-Policy/mean-features");
	m_pSigmaFeatures = new CFeatureList("Sto-Policy/sigma-features");

	m_meanAddList = new CFeatureList("meanAddList");
	m_sigmaAddList = new CFeatureList("sigmaAddList");
}

CStochasticPolicyGaussianNoise::~CStochasticPolicyGaussianNoise()
{
	delete m_pMeanFeatures;
	delete m_pSigmaFeatures;
}

double CStochasticPolicyGaussianNoise::selectAction(const CState *s, CAction *a)
{
	double mean = m_pMeanVFA->get(s);
	double sigma = 0.0;
	double output = mean;
	if (!CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//Training: add noise
		sigma = exp(m_pSigmaVFA->get(s));
		output = CGaussianNoise::getNormalDistributionSample(mean, sigma); //TODO: UNDO!
	}

	a->set(m_outputActionIndex.get(), output);

	cout << mean << "\t" << sigma << "\n";
	return CGaussianNoise::getSampleProbability(mean, sigma, output);
}

double CStochasticPolicyGaussianNoise::getOutput(const CState *s)
{
	double mean = m_pMeanVFA->get(s);
	double sigma = 0.0;
	double output = mean;
	if (!CSimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//Training: add noise
		sigma = exp(m_pSigmaVFA->get(s));
		output += CGaussianNoise::getNormalDistributionSample(mean, sigma);
	}

	return output;
}

double CStochasticPolicyGaussianNoise::getProbability(const CState *s, const CState *a, bool bStochastic)
{
	double mean = m_pMeanVFA->get(s);

	if (bStochastic && CSimionApp::get()->pSimGod->useSampleImportanceWeights())
	{
		double noise = a->get(m_outputActionIndex.get()) - mean;
		return CGaussianNoise::getSampleProbability(mean, exp(m_pSigmaVFA->get(s)), noise);
	}
	return 1.0;
}

//according to https://hal.inria.fr/hal-00764281/document
void CStochasticPolicyGaussianNoise::getNaturalGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient)
{
	m_pMeanVFA->getFeatures(s, m_pMeanFeatures);
	m_pSigmaVFA->getFeatures(s, m_pSigmaFeatures);
	double sigma = exp(m_pSigmaVFA->get(m_pSigmaFeatures));
	double mean = m_pMeanVFA->get(m_pSigmaFeatures);

	//a. Grad_u_mean pi(a|s)/pi(a|s) = (a - pi(s)) * phi_mean(s) / sigma*2
	double noise = a->get(m_outputActionIndex.get()) - mean;

	double factor = noise / (sigma*sigma);
	pOutGradient->addFeatureList(m_pMeanFeatures, factor);

	//b. Grad_u_sigma pi(a|s)/pi(a|s) = (a - pi(s))^2 -1 * phi_sigma(s) / sigma*2
	factor = (noise*noise) / (sigma*sigma) - 1.0;
	pOutGradient->addFeatureList(m_pSigmaFeatures, factor);

	cout << "\t\t\t" <<sigma << " " << noise << " " << factor << "\n";
}

void CStochasticPolicyGaussianNoise::getFeatures(const CState* state, CFeatureList* outFeatureList)
{
	m_pMeanVFA->getFeatures(state, outFeatureList);
	m_pSigmaVFA->getFeatures(state, m_pSigmaFeatures);
	outFeatureList->addFeatureList(m_pSigmaFeatures);
}
void CStochasticPolicyGaussianNoise::addFeatures(const CFeatureList* pFeatureList, double factor)
{
	m_meanAddList->clear();
	m_sigmaAddList->clear();

	unsigned int m_numWeights = CSimGod::getGlobalStateFeatureMap().get()->getTotalNumFeatures();
	//assuming that the first half of the entries correspons to the mean, and the last half to the std.dev
	pFeatureList->split(m_meanAddList, m_sigmaAddList, m_numWeights);

	m_pMeanVFA->add(m_meanAddList);
	m_pSigmaVFA->add(m_sigmaAddList);
	//m_pMeanVFA->add(pFeatureList, factor);
}
double CStochasticPolicyGaussianNoise::getDeterministicOutput(const CFeatureList* pFeatureList)
{
	return m_pMeanVFA->get(pFeatureList);
}