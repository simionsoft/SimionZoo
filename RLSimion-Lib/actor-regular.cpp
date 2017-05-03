#include "stdafx.h"
#include "policy.h"
#include "vfa.h"
#include "named-var-set.h"
#include "features.h"
#include "etraces.h"
#include "config.h"
#include "policy-learner.h"
#include "parameters-numeric.h"

CRegularPolicyGradientLearner::CRegularPolicyGradientLearner(CConfigNode* pConfigNode)
	: CPolicyLearner(pConfigNode)
{
	m_pStateFeatures = new CFeatureList("Actor/s");
	m_e= CHILD_OBJECT<CETraces>(pConfigNode, "E-Traces", "Eligibility traces used by the regular Pol.Grad. learner", true);
	m_e->setName("Actor/E-Traces");
	m_pAlpha = CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "Alpha", "The learning gain");
}

CRegularPolicyGradientLearner::~CRegularPolicyGradientLearner()
{
	delete m_pStateFeatures;
}

void CRegularPolicyGradientLearner::update(const CState *s, const CAction *a, const CState *s_p, double r, double td)
{
	double lastNoise;
	double alpha;

	//Regular gradient actor update
	//theta= theta + alpha*(lastNoise)*phi_pi(s)*td

	alpha = m_pAlpha->get();


	m_pPolicy->getFeatures(s, m_pStateFeatures);

	lastNoise = a->get(m_pPolicy->getOutputActionIndex()) - m_pPolicy->getDeterministicOutput(m_pStateFeatures);// m_pOutput->getSample(i);


	if (alpha != 0.0)
		m_pPolicy->addFeatures(m_pStateFeatures, alpha*lastNoise*td);
}

