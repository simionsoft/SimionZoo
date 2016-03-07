#include "stdafx.h"
#include "vfa-actor.h"
#include "vfa.h"
#include "named-var-set.h"
#include "features.h"
#include "etraces.h"
#include "parameters.h"
#include "vfa-policy.h"

CRegularPolicyGradientLearner::CRegularPolicyGradientLearner(CParameters *pParameters)
: CSingleOutputVFAPolicyLearner(pParameters->getChild("VFA-Policy"))
{
	m_pStateFeatures = new CFeatureList("Actor\\s");
	m_e = new CETraces("Actor\\E-Traces",pParameters->getChild("E-Traces"));
	m_pAlpha = pParameters->getNumericHandler("Alpha");
}

CRegularPolicyGradientLearner::~CRegularPolicyGradientLearner()
{
	delete m_pStateFeatures;
	delete m_e;
	delete m_pAlpha;
}

void CRegularPolicyGradientLearner::updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r, double td)
{
	double lastNoise;
	double alpha;

	//Regular gradient actor update
	//theta= theta + alpha*(lastNoise)*phi_pi(s)*td

	alpha = m_pAlpha->getValue();


	m_pPolicy->getVFA()->getFeatures(s, m_pStateFeatures);

	lastNoise = a->getValue(m_pPolicy->getOutputActionIndex()) - m_pPolicy->getVFA()->getValue(m_pStateFeatures);// m_pOutput->getValue(i);


	if (alpha != 0.0)
		m_pPolicy->getVFA()->add(m_pStateFeatures, alpha*lastNoise*td);
}

