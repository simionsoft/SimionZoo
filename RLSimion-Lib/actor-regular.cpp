#include "stdafx.h"
#include "policy.h"
#include "vfa.h"
#include "named-var-set.h"
#include "features.h"
#include "etraces.h"
#include "parameters.h"
#include "policy-learner.h"
#include "globals.h"
#include "parameters-numeric.h"

CLASS_CONSTRUCTOR(CRegularPolicyGradientLearner)(CParameters *pParameters)
	: EXTENDS(CPolicyLearner,pParameters)
{
	m_pStateFeatures = new CFeatureList("Actor/s");
	CHILD_CLASS(m_e,"E-Traces",CETraces,pParameters,"Actor/E-Traces");
	NUMERIC_VALUE(m_pAlpha,pParameters,"Alpha");

	END_CLASS();
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

