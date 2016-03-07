#include "stdafx.h"
#include "named-var-set.h"
#include "noise.h"
#include "vfa.h"
#include "vfa-policy.h"
#include "parameters.h"
#include "globals.h"
#include "world.h"
#include "features.h"

CSingleOutputVFAPolicy* CSingleOutputVFAPolicy::getInstance(CParameters* pParameters)
{
	const char* pName = pParameters->getChild()->getName();

	if (!strcmp(pName, "Deterministic-Policy-Gaussian-Noise"))
		return new CDeterministicPolicyGaussianNoise(pParameters->getChild());
	if (!strcmp(pName, "Stochastic-Policy-Gaussian-Noise"))
		return new CStochasticPolicyGaussianNoise(pParameters->getChild());

	return 0;
}

CSingleOutputVFAPolicy::CSingleOutputVFAPolicy(CParameters* pParameters)
: CParamObject(pParameters)
{
	m_pVFA = new CLinearStateVFA(pParameters->getChild("Linear-State-VFA"));

	m_outputAction = pParameters->getConstString("Output-Action");

	CAction *pActionDescriptor = RLSimion::g_pWorld->getActionDescriptor();
	m_outputActionIndex = pActionDescriptor->getVarIndex(m_outputAction);

	m_pVFA->saturateOutput(pActionDescriptor->getMin(m_outputActionIndex), pActionDescriptor->getMax(m_outputActionIndex));
}

CSingleOutputVFAPolicy::~CSingleOutputVFAPolicy()
{
	delete m_pVFA;
}



//CDetPolicyGaussianNoise////////////////////////////////
/////////////////////////////////////////////////////////

CDeterministicPolicyGaussianNoise::CDeterministicPolicyGaussianNoise(CParameters* pParameters)
: CSingleOutputVFAPolicy(pParameters)
{
	m_pExpNoise = CNoise::getInstance(pParameters->getChild("Exploration-Noise"));
}

CDeterministicPolicyGaussianNoise::~CDeterministicPolicyGaussianNoise()
{
	delete m_pExpNoise;
}

void CDeterministicPolicyGaussianNoise::getGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient)
{
	//0. Grad_u pi(a|s)/pi(a|s) = (a - pi(s)) * phi(s) / sigma*2
	m_pVFA->getFeatures(s, pOutGradient);

	//TXAPUZAAAA^2!!!
	double sigma = ((CGaussianNoise*)m_pExpNoise)->getSigma();

	double noise = a->getValue(m_outputActionIndex) - m_pVFA->getValue((const CFeatureList*)pOutGradient);

	double unscaled_noise = ((CGaussianNoise*)m_pExpNoise)->unscale(noise);
	double factor = unscaled_noise / (sigma*sigma);
}

void CDeterministicPolicyGaussianNoise::selectAction(const CState *s, CAction *a)
{
	double noise;
	double output;

	noise = m_pExpNoise->getValue();

	output = m_pVFA->getValue(s);

	a->setValue(m_outputActionIndex, output + noise);
}

//CStoPolicyGaussianNoise//////////////////////////
////////////////////////////////////////////////

CStochasticPolicyGaussianNoise::CStochasticPolicyGaussianNoise(CParameters* pParameters) : CSingleOutputVFAPolicy(pParameters)
{
	m_pSigmaVFA = new CLinearStateVFA(m_pVFA->getParameters());//same parameterization as the mean-VFA
	m_pAux = new CFeatureList("Sto-Policy\\aux");
}

CStochasticPolicyGaussianNoise::~CStochasticPolicyGaussianNoise()
{
	delete m_pSigmaVFA;
	delete m_pAux;
}

void CStochasticPolicyGaussianNoise::selectAction(const CState *s, CAction *a)
{
	double sigma;
	double mean;
	double output;

	sigma = m_pSigmaVFA->getValue(s);

	mean = m_pVFA->getValue(s);

	output = getNormalDistributionSample(mean, sigma);

	a->setValue(m_outputActionIndex, output);
}

//returns the factor by which the state features have to be multiplied to get the policy gradient
void CStochasticPolicyGaussianNoise::getGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient)
{

}