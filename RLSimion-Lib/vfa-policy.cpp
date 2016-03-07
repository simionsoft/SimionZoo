#include "stdafx.h"
#include "named-var-set.h"
#include "noise.h"
#include "vfa.h"
#include "vfa-policy.h"
#include "parameters.h"
#include "globals.h"
#include "world.h"


CSingleOutputVFAPolicy::CSingleOutputVFAPolicy(CParameters* pParameters)
: CParamObject(pParameters)
{
	m_pVFA = new CLinearStateVFA(pParameters->getChild("Linear-State-VFA"));

	m_pExpNoise = CNoise::getInstance (pParameters->getChild("Exploration-Noise"));

	m_outputAction = pParameters->getConstString("Output-Action");

	CAction *pActionDescriptor = RLSimion::g_pWorld->getActionDescriptor();
	m_outputActionIndex = pActionDescriptor->getVarIndex(m_outputAction);

	m_pVFA->saturateOutput(pActionDescriptor->getMin(m_outputActionIndex), pActionDescriptor->getMax(m_outputActionIndex));
}

CSingleOutputVFAPolicy::~CSingleOutputVFAPolicy()
{
	delete m_pVFA;
	delete m_pExpNoise;
}

void CSingleOutputVFAPolicy::selectAction(const CState *s, CAction *a)
{
	double noise;
	double output;
	
	noise = m_pExpNoise->getValue();

	output = m_pVFA->getValue(s);

	a->setValue(m_outputActionIndex, output + noise);
}

void CSingleOutputVFAPolicy::getGradient(const CState* s, const CAction* a, CFeatureList* pOutGradient)
{
	//0. Grad_u pi(a|s)/pi(a|s) = (a - pi(s)) * phi(s) / sigma*2
	m_pVFA->getFeatures(s, pOutGradient);

	//TXAPUZAAAA^2!!!
	double sigma = ((CGaussianNoise*)m_pExpNoise)->getSigma();
	
	double noise = a->getValue(m_outputActionIndex) - m_pVFA->getValue((const CFeatureList*)pOutGradient);

	double unscaled_noise = ((CGaussianNoise*)m_pExpNoise)->unscale(noise);
	double factor = unscaled_noise / (sigma*sigma);
}